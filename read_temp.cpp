/*
 * Read Temp: Reads temperature and contacts server with upload. Keeps local data if server contact fails.
 */

#include <iostream>
#include <string>
#include <vector>
#include "lib/one_wire_sensor/OneWireSensor.h"
#include "lib/command_line_parser/CommandLineParser.h"
#include "lib/read_rpi_board/read_rpi_board.h"
#include "lib/server_communication/SendTempToServer.h"

using namespace std;

int main(int argc, char* argv[]) {

  OneWireSensor temp_sensor;
  CommandLineParser cl;
  SendTempToServer tempsession;

  // Parse command line options
  if (!cl.parse_cl_opt_rt(argc, argv)) {
    return EXIT_FAILURE;
  }

  //Read database authentication info
  if (!tempsession.load_db_param()) {
    return EXIT_FAILURE;
  }

  //Get locally stored remote info
  if (cl.options.verbose) cout << cl.info.reading_rem_info << endl;

  if (!tempsession.get_remote_info()) {
    cerr << cl.errors.error_reading_rem_info << endl;
    return EXIT_FAILURE;
  }

  tempsession.remote.server_address += "/api/save_temp";

  if (!get_rpi_serial(&tempsession.remote.board_serial)) {
    cerr << cl.errors.error_reading_board_serial << endl;
  }

  if (cl.options.measure_temp) {
    if (cl.options.verbose) cout << cl.info.reading_sensor << endl;
    temp_sensor.read_temp(tempsession.remote.sensor_directory, tempsession.remote.sensor_serial);
    if (temp_sensor.sensor_temp == 999) {
      cerr << cl.errors.error_reading_temp << endl;
      return EXIT_FAILURE;
    }

    if (cl.options.verbose) cout << cl.info.sensor_read;
    if (cl.options.print) cout << temp_sensor.sensor_temp << " \370C" << endl;

    if (cl.options.save_local) {
      if (cl.options.verbose) cout << cl.info.saving_to_local << endl;
      if (!tempsession.save_temp(temp_sensor.sensor_temp)) {
        cerr << cl.errors.error_storing_local << endl;
        return EXIT_FAILURE;
      }
        if (cl.options.verbose) cout << cl.info.temp_saved_local << endl;
    }
  }

  if (!cl.options.send_to_server) {
    return EXIT_SUCCESS;
  }

  if (!cl.options.save_local && cl.options.measure_temp) {
    //Only send last temp
    SendTempToServer::saved_temp local_meas;
    local_meas.temp = temp_sensor.sensor_temp;
    local_meas.timestamp = tempsession.sql_timestamp();
    local_meas.id = 0;
    tempsession.local_temps.push_back(local_meas);
  } else {
    if (cl.options.verbose) cout << cl.info.getting_local_stored << endl;
    tempsession.local_temps = tempsession.get_saved_temperatures();
  }

  if (tempsession.local_temps.size() == 0) {
    if (cl.options.print) cout << cl.info.no_send_exit << endl;
    return EXIT_SUCCESS;
  }

  if (cl.options.verbose) {
    cout << cl.info.data_to_send << endl;
    for (vector<SendTempToServer::saved_temp>::iterator it = tempsession.local_temps.begin(); it != tempsession.local_temps.end(); it++) {
      cout << "ID: " << it->id << " TEMP: " << it->temp << " TIME: " << it->timestamp << endl;
    }
  }

  //Send to server
  tempsession.generate_server_temperature_message();
  //Encode server message
  tempsession.url_encode(tempsession.server_message.dump());

  if (cl.options.verbose) cout << cl.info.sending << endl;

  if (!tempsession.post_to_server()) {
    cerr << cl.errors.error_failed_server_contact << endl;
    return EXIT_FAILURE;
  };

  if (!tempsession.parse_saved_temperatures()) {
    cerr << cl.errors.error_unexpected_server_response << endl;
    return EXIT_FAILURE;
  }

  if (tempsession.server_response_code == 1) {
    if (cl.options.print) cout << cl.info.server_save_ok << endl;
    if (cl.options.verbose) cout << cl.info.server_msg << tempsession.server_response_msg << endl;

    if (cl.options.save_local) {
      //Option remote was used. Do not look in db.
      if (cl.options.verbose) cout << cl.info.removing_local << endl;
      if (tempsession.remove_temps(tempsession.sql_auth, tempsession.temps_saved_on_server)) {
        if (cl.options.verbose) cout << cl.info.removed << endl;
        return EXIT_SUCCESS;
      }
    } return EXIT_SUCCESS;
  } else {
    if (cl.options.print) cout << cl.errors.error_failed_server_save << endl;
    if (cl.options.verbose) cout << cl.info.server_msg << tempsession.server_message << endl;
    if (cl.options.verbose) cout << cl.info.local_storage_kept << endl;
    return EXIT_FAILURE;
  }
  return EXIT_FAILURE;
}
