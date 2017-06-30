/*
 * Read Temp: Reads temperature and contacts server with upload. Keeps local data if server contact fails.
 */

//Sensor read
#include "lib/OneWireSensor.h"
//Command Line parsing
#include "lib/cl_parser.h"
//Messages
#include "lib/messages_eng.h"
//Board Info
#include "lib/read_rpi_board.h"

#include "lib/SendTempToServer.h"

#include <iostream>
#include <string.h>
#include <vector>


using namespace std;

int main(int argc, char* argv[]) {

    vector<SendTempToServer::saved_temp> saved_temps, temps_saved_on_server;
    OneWireSensor temp_sensor;
    cl_opt cl;
    output_messages msg;

    SendTempToServer tempsession;

    // Parse command line options
    if (!parse_cl_opt(argc, argv, &cl)) {
        return EXIT_FAILURE;
    }

    //Read database authentication info
    if (!tempsession.load_db_param(&tempsession.sql_auth)) {
        return EXIT_FAILURE;
    }

    //Get locally stored remote info
    if (cl.verbose) cout << msg.reading_rem_info << endl;

    if (!tempsession.get_remote_info(&tempsession.sql_auth, &tempsession.remote)) {
        cerr << msg.error_reading_rem_info << endl;
        return EXIT_FAILURE;
    }

    tempsession.remote.server_address += "/api/save_temp";

    if (!get_rpi_serial(&tempsession.remote.board_serial)) {
        cerr << msg.error_reading_board_serial << endl;
    }

    if (cl.measure_temp) {
        if (cl.verbose) cout << msg.reading_sensor << endl;
        temp_sensor.read_temp(tempsession.remote.sensor_directory, tempsession.remote.sensor_serial);
        if (temp_sensor.sensor_temp == 999) {
            cerr << msg.error_reading_temp << endl;
            return EXIT_FAILURE;
        }

        if (cl.verbose) cout << msg.sensor_read;
        if (cl.print) cout << temp_sensor.sensor_temp << " \370C" << endl;

        if (cl.save_local) {
            if (cl.verbose) cout << msg.saving_to_local << endl;
            if (!tempsession.save_temp(temp_sensor.sensor_temp, &tempsession.sql_auth)) {
                cerr << msg.error_storing_local << endl;
                return EXIT_FAILURE;
            }
            if (cl.verbose) cout << msg.temp_saved_local << endl;
        }
    }

    if (!cl.send_to_server) {
        return EXIT_SUCCESS;
    }

    if (!cl.save_local && cl.measure_temp) {
        //Only send last temp
        SendTempToServer::saved_temp local_meas;
        local_meas.temp = temp_sensor.sensor_temp;
        local_meas.timestamp = tempsession.sql_timestamp();
        local_meas.id = 0;
        saved_temps.push_back(local_meas);
    } else {
        if (cl.verbose) cout << msg.getting_local_stored << endl;
        saved_temps = tempsession.get_saved_temperatures(&tempsession.sql_auth);
    }

    if (saved_temps.size() == 0) {
        if (cl.print) cout << msg.no_send_exit << endl;
        return EXIT_SUCCESS;
    }

    if (cl.verbose) {
        cout << msg.data_to_send << endl;
        for (vector<SendTempToServer::saved_temp>::iterator it = saved_temps.begin(); it != saved_temps.end(); it++) {
            cout << "ID: " << it->id << " TEMP: " << it->temp << " TIME: " << it->timestamp << endl;
        }
    }

    //Send to server
    //SendTempToServer server_session(remote, saved_temps);
    tempsession.local_temps = saved_temps;
    tempsession.generate_server_temperature_message();
    tempsession.url_encode(tempsession.server_message.dump());
    //Encode server message
    //url_encode(server_message.dump());
    if (cl.verbose) cout << msg.sending << endl;

    if (!tempsession.post_to_server(tempsession.encoded_post, tempsession.remote.server_address)) {
        cerr << msg.error_failed_server_contact << endl;
        return EXIT_FAILURE;
    };
    if (!tempsession.parse_saved_temperatures()) {
        cerr << msg.error_unexpected_server_response << endl;
        return EXIT_FAILURE;
    }

    //TODO ERROR HANDLING FOR THIS STRING ??

    if (tempsession.server_response_code == 1) {
        if (cl.print) cout << msg.server_save_ok << endl;
        if (cl.verbose) cout << msg.server_msg << tempsession.server_response_msg << endl;

        if (cl.save_local) {
            //Option remote was used. Do not look in db.
            if (cl.verbose) cout << msg.removing_local << endl;
            if (tempsession.remove_temps(tempsession.sql_auth, tempsession.temps_saved_on_server)) {
                if (cl.verbose) cout << msg.removed << endl;
                return EXIT_SUCCESS;
            }
        } return EXIT_SUCCESS;
    } else {
        if (cl.print) cout << msg.error_failed_server_save << endl;
        if (cl.verbose) cout << msg.server_msg << tempsession.server_message << endl;
        if (cl.verbose) cout << msg.local_storage_kept << endl;
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}