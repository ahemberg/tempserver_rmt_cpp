//
// Created by A547832 on 2017-06-20.
//

/*
 * Todo: an implementation of CPU usage. As of now a little bit tricky: Different units have different number
 * of cores. How do we save that to the server?
 */

#include <iostream>
#include <string>

#include "lib/nlohmann_json/json.hpp"
#include "lib/server_communication/SendStatusToServer.h"

using namespace std;

int main() {

  SendStatusToServer foo;

  //Read database authentication info
  if (!foo.load_db_param()) {
    return EXIT_FAILURE;
  }

  //Read remote info from db
  if (!foo.get_remote_info()) {
    return EXIT_FAILURE;
  }

  foo.remote.server_address += "/api/update_status";

  //Get RPI Serial
  if (!foo.get_rpi_serial(&foo.remote.board_serial)) {
    cerr << "Failed to get board serial" << endl;
  } else {
    cout << "Board serial: " << foo.remote.board_serial << endl;
  }

  //Get CPU temp
  if (!foo.get_cpu_temp(&foo.new_measurement.cpu)) {
    cerr << "Failed to get cpu temp" << endl;
  }

  //Get CPU load
  if (!foo.get_cpu_load(&foo.new_measurement.cpu)) {
    cerr << "Failed to get cpu load" << endl;
  };

  if(!foo.get_ram_info(&foo.new_measurement.memory)) {
    cerr << "Failed to get all memory parameters" << endl;
  }

  //Get hdd usage
  if (!foo.get_hdd_usage(&foo.new_measurement.disk)) {
    cerr << "Failed to get disk info" << endl;
  }

  foo.ping_server(&foo.new_measurement.network);

  //Get measurement timestamp and set id to 0
  //foo.new_measurement.timestamp = foo.sql_timestamp();
  //foo.new_measurement.id = 0; SEE TODO BELOW

  //TODO: This is heavy on the memory card. Will kill it after a year or two
  //TODO: Do not save if not necessary Save only on fail

  //Insert measurement to dB
  foo.save_status_message();

  //Retrieve all measurements from dB
  foo.measurements_to_send = foo.get_saved_status_messages();

  foo.generate_server_status_message();

  foo.url_encode(foo.server_message.dump());

  foo.post_to_server(foo.encoded_post);

  foo.parse_saved_messages();

  cout << "Server status" << endl;
  cout << foo.server_response_code << endl;

  cout << "Server Message" << endl;
  cout << foo.server_response_msg << endl;

  if (foo.server_response_code == 1) {
    cout << "Server contact OK. Removing messages saved on server" << endl;
    foo.remove_status_messages(foo.server_saved_messages);
    return EXIT_SUCCESS;
  } else {
    cerr << "Server contact FAIL. Local storage has been kept" << endl;
    cerr << foo.server_response_code << endl;
    cerr << foo.server_response_msg << endl;
  }

  return EXIT_FAILURE;
}
