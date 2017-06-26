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
    SendStatusToServer send;
    board_parameters board;
    db_auth sql_auth;
    string serial;

    //Read database authentication info
    if (!load_db_param(&sql_auth)) {
        return EXIT_FAILURE;
    }

    //Get RPI Serial
    if (!get_rpi_serial(&serial)) {
        cerr << "Failed to get board serial" << endl;
    } else {
        cout << "Board serial: " << serial << endl;
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

    /*Old procedure:
     * 1 measure
     * 2 save local
     * 3 retrieve all local
     * 4 send to server
     * 5a remove saved
     * 5b keep non saved
     * Alternative Procedure:
     * 1 measure
     * 2 retrieve local
     * 3 assemble old with new
     * 4 send
     * 5a remove saved
     * 5b keep non saved
     * 5c save current measurement if not saved on server
     *
     * PROS:
     * No write to SD if all is successful!
     * Probably faster
     * CON:
     * Measurement lost if there are any interruptions
     * Need to handle no answer properly!
     * Implementation more complicated (Maybe this is a PRO?)
     */


    nlohmann::json board_info = send.create_status_blob(&board);

    cout << board_info.dump(1) << endl;

    //Get available disk-space
    return EXIT_SUCCESS;
}

