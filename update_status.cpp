//
// Created by A547832 on 2017-06-20.
//

/*
 * Todo: an implementation of CPU usage. As of now a little bit tricky: Different units have different number
 * of cores. How do we save that to the server?
 */

#include <iostream>
#include <string>

#include "lib/structs.h"
#include "lib/read_rpi_board.h"


using namespace std;

int main() {
    board_parameters board;
    string serial;
    double cpu_temp;

    //Get RPI Serial
    if (!get_rpi_serial(&serial)) {
        cerr << "Failed to get board serial" << endl;
    } else {
        cout << "Board serial: " << serial << endl;
    }

    //Get CPU temp
    if (!get_cpu_temp(&cpu_temp)) {
        cerr << "Failed to get cpu temp" << endl;
    } else {
        cout << "CPU Temp: " << cpu_temp << " \370C" << endl;
    }


    if(!get_ram_info(&board.memory)) {
        cerr << "Failed to get all memory parameters" << endl;
    } else {
        std::cout << "Memory status:" << std::endl;
        std::cout << "Total:" << board.memory.mem_total/1e6 << " Free:" << board.memory.mem_free/1e6 << " Used: " << (board.memory.total_used)/1e6 << "MB" << std::endl;
        std::cout << "Non cache/buffer memory: " << board.memory.non_cabuf_mem/1e6 << " MB" << std::endl;
        std::cout << "Buffers: " << board.memory.buffers/1e6 << " MB" << std::endl;
        std::cout << "Cached: " << board.memory.cache/1e6 << " MB" << std::endl;
        std::cout << "Swap: " << board.memory.swap/1e6 << " MB" << std::endl;
    }

    //Get hdd usage
    if (!get_hdd_usage(&board.disk)) {
        cerr << "Failed to get disk info" << endl;
    } else {
        std::cout << "File system Usage:" << std::endl;
        std::cout << "Size " << board.disk.size/1e6 << " MB" << std::endl;
        std::cout << "Used " << board.disk.used/1e6 << " MB" << std::endl;
        std::cout << "Avail " << board.disk.available/1e6 << " MB" << std::endl;
        std::cout << "Used " << board.disk.used_p*100 << "%" << std::endl;
    }

    //Assemble and send to server


    //Get available disk-space
    return EXIT_SUCCESS;
}

