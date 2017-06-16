#include "pch.h"

//Sensor read
#include "OneWireSensor.h"
//Server Comm
#include "TalkToServer.h"

//Database Functions
#include "dbFunctions.h"

//Option parsing (move to pch later)
//#include "unistd.h"

using namespace std;
using json = nlohmann::json;

static void show_usage(std::string name)
{
    //TODO: Implement long options too
    std::cerr << "Usage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "-h \t\tShow this help message\n"
              << "-s [option]\tControl how the temperature is read and saved.\n"
              << "\t\tlocal  - Measure temp and save locally. Do not send to server\n"
              << "\t\tremote - Measure temp and send to server, do not save locally if server call fails,\n"
              << "\t\t\t does not send any previously saved measurements\n"
              << "\t\tno-save - Only measure temp, do not save or send. Prints to stdout\n"
              << "\t\tsend-only - Sends saved temperatures, does not measure temp\n"
              << "-o [option]\tControl output to stdout\n"
              << "\t\tverbose - Verbose printing to stdout\n"
              << "\t\tquiet - Prints nothing\n"
              << std::endl;
}

struct cl_opt {
    bool send_to_server = true,
         measure_temp = true,
         save_local = true,
         print = true,
         verbose = false;
};

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

bool parse_cl_opt(int c, char *v[], cl_opt* options) {
    int opt;
    while ((opt = getopt(c, v, "s:o:h")) != -1) {
        switch (opt) {
            case 'h':
                show_usage(v[0]);
                return false;
            case 's':
                if (!strcmp(optarg, "local")) {
                    options->send_to_server = false;
                } else if (!strcmp(optarg, "remote")) {
                    options->save_local = false;
                } else if (!strcmp(optarg, "no-save")) {
                    options->send_to_server = false;
                    options->save_local = false;
                } else if (!strcmp(optarg, "send-only")) {
                    options->measure_temp = false;
                } else {
                    cout << "Invalid option: " << optarg << endl;
                    show_usage(v[0]);
                    return false;
                }
                break;
            case 'o':
                if (!strcmp(optarg, "verbose")) {
                    options->print = true;
                    options->verbose = true;
                } else if (!strcmp(optarg, "quiet")) {
                    options->print = false;
                } else {
                    cout << "Invalid option: " << optarg << endl;
                    show_usage(v[0]);
                    return false;
                }
                break;
            default:
                show_usage(v[0]);
                return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {

    string sensor_serial, server_response_raw, urlencode_post;
    temperature_vector saved_temps, temps_saved_on_server;
    json server_message_json;
    remote_info remote;
    db_auth sql_auth;
    OneWireSensor temp_sensor;
    cl_opt cl;

    // Parse command line options
    if (!parse_cl_opt(argc, argv, &cl)) {
        cout << "asdölkasdö" << endl;
        return EXIT_FAILURE;
    }

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    };

    //Get locally stored remote info
    if (cl.verbose) cout << "Reading remote info from database..." << endl;

    if (!get_remote_info(&sql_auth, &remote)) {
        perror("Failed to get remote info from database");
        return EXIT_FAILURE;
    }
    remote.board_serial = get_rpi_serial();

    if (cl.measure_temp) {
        if (cl.verbose) cout << "Reading sensor..." << endl;
        temp_sensor.read_temp(remote.sensor_directory, remote.sensor_serial);
        if (temp_sensor.sensor_temp == 999) {
            perror("Unable to read temperature, Exiting.");
            return EXIT_FAILURE;
        }

        if (cl.verbose) cout << "Sensor read! Temperature is ";
        if (cl.print) cout << temp_sensor.sensor_temp << " \370C" << endl;

        if (cl.save_local) {
            if (cl.verbose) cout << "Saving temp to local storage..." << endl;
            if (!save_temp(temp_sensor.sensor_temp, &sql_auth)) {
                perror("Unable to store to local database. Aborting");
                return EXIT_FAILURE;
            }
            if (cl.verbose) cout << "Temp saved to local storage." << endl;
        }
    }

    if (!cl.send_to_server) {
        //Exit if -l flag was used
        return EXIT_SUCCESS;
    }

    if (cl.verbose) cout << "Getting locally stored measurements.."  << endl;

    if (!cl.save_local && cl.measure_temp) {
        //Only send last temp
        // TODO: HERE. Must implement this
        /*saved_temps.push_back(0);
        saved_temps[0].temp = temp_sensor.sensor_temp;
        saved_temps[0].timestamp = "<GET TIMESTAMP>";
        saved_temps[0].id = 0;*/
        cout << "This option is not implemented" << endl;
    } else {
        saved_temps = get_saved_temperatures(&sql_auth);
    }


    if (saved_temps.size() == 0) {
        if (cl.print) cout << "Nothing to send, exiting" << endl;
        return EXIT_SUCCESS;
    }

    //Send to server
    TalkToServer server_session(remote, saved_temps);
    if (cl.verbose) cout << "Sending to server..." << endl;

    if (!server_session.post_to_server(server_session.encoded_post)) {
        perror("Failed to contact server. Local storage has been kept.");
        return EXIT_FAILURE;
    };
    server_session.parse_server_response();

    //TODO ERROR HANDLING FOR THIS STRING ??

    if (server_session.server_response_code == 1) {
        if (cl.print) cout << "Server successfully saved temperatures." << endl;
        if (cl.verbose) cout << "Server message: " << server_session.server_response_msg << endl;
        if (cl.verbose) cout << "Removing data saved on server from local storage.." << endl;

        if (remove_temps(sql_auth, server_session.temps_saved_on_server)) {
            if (cl.verbose) cout << "Local data removed" << endl;
            return EXIT_SUCCESS;
        }
    } else {
        if (cl.print) cout << "Server failed to save temperatures" << endl;
        if (cl.verbose) cout << "Server message: " << server_session.server_message << endl;
        if (cl.verbose) cout << "Local storage has been kept." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}

#pragma clang diagnostic pop