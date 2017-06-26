//
// Created by A547832 on 2017-06-20.
//

/*
 * Todo: an implementation of CPU usage. As of now a little bit tricky: Different units have different number
 * of cores. How do we save that to the server?
 */

#include <iostream>
#include <string>

#include "lib/json.hpp"

#include "lib/structs.h"
#include "lib/read_rpi_board.h"
#include "lib/dbFunctions.h"
#include "lib/SendStatusToServer.h"


using namespace std;

int main() {
    board_parameters board;
    vector<board_parameters> saved_measurements, received_measurements;
    remote_info remote;
    db_auth sql_auth;
    string serial;

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    }

    //Read remote info from db
    if (!get_remote_info(&sql_auth, &remote)) {
        return EXIT_FAILURE;
    }

    remote.server_address += "/api/update_status";

    //Get RPI Serial
    if (!get_rpi_serial(&remote.board_serial)) {
        cerr << "Failed to get board serial" << endl;
    } else {
        cout << "Board serial: " << remote.board_serial << endl;
    }

    //Get CPU temp
    if (!get_cpu_temp(&board.cpu)) {
        cerr << "Failed to get cpu temp" << endl;
    }

    //Get CPU load
    if (!get_cpu_load(&board.cpu)) {
        cerr << "Failed to get cpu load" << endl;
    };

    if(!get_ram_info(&board.memory)) {
        cerr << "Failed to get all memory parameters" << endl;
    }

    //Get hdd usage
    if (!get_hdd_usage(&board.disk)) {
        cerr << "Failed to get disk info" << endl;
    }

    ping_server(&board.network);

    //Get measurement timestamp and set id to 0
    board.timestamp = sql_timestamp();
    board.id = 0;

    //TODO: This is heavy on the memory card. Will kill it after a year or two
    //TODO: Do not save if not necessary Save only on fail

    //Insert measurement to dB
    save_status_message(&board, &sql_auth);

    //Retrieve all measurements from dB
    saved_measurements = get_saved_status_messages(&sql_auth);

    SendStatusToServer send(remote, saved_measurements);

    send.post_to_server(send.encoded_post);
    send.parse_saved_messages();

    cout << "Server status" << endl;
    cout << send.server_response_code << endl;

    cout << "Server Message" << endl;
    cout << send.server_response_msg << endl;

    if (send.server_response_code == 1) {
        cout << "Server contact OK. Removing messages saved on server" << endl;
        remove_status_messages(&sql_auth, send.server_saved_messages);
        return EXIT_SUCCESS;
    } else {
        cerr << "Server contact FAIL. Local storage has been kept" << endl;
        cerr << send.server_response_code << endl;
        cerr << send.server_response_msg << endl;
    }

    return EXIT_FAILURE;
}

