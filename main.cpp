#include "pch.h"

//Sensor read
#include "OneWireSensor.h"
//Server Comm
#include "TalkToServer.h"

//Database Functions
#include "dbFunctions.h"

using namespace std;
using json = nlohmann::json;

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

    cout << path << endl;

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

int main() {
    string sensor_serial, server_response_raw, urlencode_post;
    temperature_vector saved_temps, temps_saved_on_server;
    json server_message_json;
    remote_info remote;
    db_auth sql_auth;
    OneWireSensor temp_sensor;

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    };

    //Get locally stored remote info
    cout << "Reading remote info from database..." << endl;

    if (!get_remote_info(&sql_auth, &remote)) {
        perror("Failed to get remote info from database");
        return EXIT_FAILURE;
    }
    remote.board_serial = get_rpi_serial();

    cout << "Reading sensor..." << endl;
    temp_sensor.read_temp(remote.sensor_directory, remote.sensor_serial);
    if (temp_sensor.sensor_temp == 999) {
        perror("Unable to read temperature, Exiting.");
        return EXIT_FAILURE;
    }
    cout << "Sensor read! Temperature is " << temp_sensor.sensor_temp << "C" << endl;

    cout << "Saving temp to local storage..." << endl;
    if (!save_temp(temp_sensor.sensor_temp, &sql_auth)) {
        perror("Unable to store to local database. Aborting");
        return EXIT_FAILURE;
    }
    cout << "Temp saved to local storage. Getting locally stored measurements.." << endl;

    saved_temps = get_saved_temperatures(&sql_auth);

    //Send to server
    TalkToServer server_session(remote, saved_temps);
    cout << "Sending to server..." << endl;

    if (!server_session.post_to_server(server_session.encoded_post)) {
        cout << "Failed to contact server." << endl;
        cout << "Local storage has been kept." << endl;
        return EXIT_FAILURE;
    };
    server_session.parse_server_response();

    //TODO ERROR HANDLING FOR THIS STRING ??

    if (server_session.server_response_code == 1) {
        cout << "Server successfully saved temperatures." << endl;
        cout << "Server message: " << server_session.server_response_msg << endl;

        cout << "Removing data saved on server from local storage.." << endl;

        if (remove_temps(sql_auth, server_session.temps_saved_on_server)) {
            cout << "Local data removed" << endl;
            return EXIT_SUCCESS;
        }
    } else {
        cout << "Server failed to save temperatures" << endl;
        cout << "Server message: " << server_session.server_message << endl;
        cout << "Local storage has been kept." << endl;
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}


#pragma clang diagnostic pop