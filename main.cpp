#include "lib/pch.h"
//Sensor read
#include "lib/OneWireSensor.h"
//Server Comm
#include "lib/TalkToServer.h"
//Database Functions
#include "lib/dbFunctions.h"

//Command Line parsing
#include "lib/cl_parser.h"

#include "lib/messages_eng.h"

using namespace std;
using json = nlohmann::json;

std::string sql_timestamp() {
    /*
     * Returns the current system timestamp in SQL Format as:
     * YYYY-MM-DD HH:MM:SS
     */

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    std::string str(buffer);

    return str;
}

//JSON FUNCTIONS

bool load_db_param(db_auth *params) {
    /*
     * Loads database authentication info from local secrets file. Params should point to a
     * db_auth struct. Secrets file has to be located in $HOME/tempserver_remote/cpp/
     *
     * Returns true if file was loaded properly, false otherwise.
     *
     */

    const char * homeDir = getenv("HOME");
    const char * filePath = "/tempserver_remote/cpp/secrets";

    char path[100];

    strcpy(path, homeDir);
    strcat(path, filePath);

    ifstream i(path);

    if (i) {
        try {
            json j;
            i >> j;
            params->host = j["database_auth"]["host"];
            params->database = j["database_auth"]["database"];
            params->user = j["database_auth"]["user"];
            params->pwd = j["database_auth"]["password"];
        } catch (domain_error &e) {
            cout << "Bad format in secrets file!" << endl;
            return false;
        }
        return true;
    } else {
        cout << "Failed to load secrets file!" << endl;
        return false;
    }
}

string get_rpi_serial() {

    ifstream i("/proc/cpuinfo");
    string line;
    string serial;

    if (!i) {
        return "0000000000000000";
    }

    while (getline(i, line)) {
        if (line.find("Serial") != string::npos) {
            serial = line.substr(line.length()- 16);
            return serial;
        }
    }
    return "00000000000000";
}

int main(int argc, char* argv[]) {

    string sensor_serial, server_response_raw, urlencode_post;
    temperature_vector saved_temps, temps_saved_on_server;
    json server_message_json;
    remote_info remote;
    db_auth sql_auth;
    OneWireSensor temp_sensor;
    cl_opt cl;
    output_messages msg;

    // Parse command line options
    if (!parse_cl_opt(argc, argv, &cl)) {
        return EXIT_FAILURE;
    }

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    }

    //Get locally stored remote info
    if (cl.verbose) cout << msg.reading_rem_info << endl;

    if (!get_remote_info(&sql_auth, &remote)) {
        cerr << msg.error_reading_rem_info << endl;
        return EXIT_FAILURE;
    }
    remote.board_serial = get_rpi_serial();

    if (cl.measure_temp) {
        if (cl.verbose) cout << msg.reading_sensor << endl;
        temp_sensor.read_temp(remote.sensor_directory, remote.sensor_serial);
        if (temp_sensor.sensor_temp == 999) {
            cerr << msg.error_reading_temp << endl;
            return EXIT_FAILURE;
        }

        if (cl.verbose) cout << msg.sensor_read;
        if (cl.print) cout << temp_sensor.sensor_temp << " \370C" << endl;

        if (cl.save_local) {
            if (cl.verbose) cout << msg.saving_to_local << endl;
            if (!save_temp(temp_sensor.sensor_temp, &sql_auth)) {
                cerr << msg.error_storing_local << endl;
                return EXIT_FAILURE;
            }
            if (cl.verbose) cout << msg.temp_saved_local << endl;
        }
    }

    if (!cl.send_to_server) {
        //Exit if -l flag was used
        return EXIT_SUCCESS;
    }

    if (!cl.save_local && cl.measure_temp) {
        //Only send last temp
        saved_temp local_meas;
        local_meas.temp = temp_sensor.sensor_temp;
        local_meas.timestamp = sql_timestamp();
        local_meas.id = 0;
        saved_temps.push_back(local_meas);
    } else {
        if (cl.verbose) cout << msg.getting_local_stored << endl;
        saved_temps = get_saved_temperatures(&sql_auth);
    }

    if (saved_temps.size() == 0) {
        if (cl.print) cout << msg.no_send_exit << endl;
        return EXIT_SUCCESS;
    }

    if (cl.verbose) {
        cout << msg.data_to_send << endl;
        for (temperature_vector::iterator it = saved_temps.begin(); it != saved_temps.end(); it++) {
            cout << "ID: " << it->id << " TEMP: " << it->temp << " TIME: " << it->timestamp << endl;
        }
    }

    //Send to server
    TalkToServer server_session(remote, saved_temps);
    if (cl.verbose) cout << msg.sending << endl;

    if (!server_session.post_to_server(server_session.encoded_post)) {
        cerr << msg.error_failed_server_contact << endl;
        return EXIT_FAILURE;
    };
    server_session.parse_server_response();

    //TODO ERROR HANDLING FOR THIS STRING ??

    if (server_session.server_response_code == 1) {
        if (cl.print) cout << msg.server_save_ok << endl;
        if (cl.verbose) cout << msg.server_msg << server_session.server_response_msg << endl;

        if (cl.save_local) {
            //Option remote was used. Do not look in db.
            if (cl.verbose) cout << msg.removing_local << endl;
            if (remove_temps(sql_auth, server_session.temps_saved_on_server)) {
                if (cl.verbose) cout << msg.removed << endl;
                return EXIT_SUCCESS;
            }
        }
    } else {
        if (cl.print) cout << msg.error_failed_server_save << endl;
        if (cl.verbose) cout << msg.server_msg << server_session.server_message << endl;
        if (cl.verbose) cout << msg.local_storage_kept << endl;
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}