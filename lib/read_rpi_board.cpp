//
// Created by A547832 on 2017-06-20.
// Functions for reading RPI board info
//

#include "read_rpi_board.h"

std::string get_rpi_serial() {

    std::ifstream i("/proc/cpuinfo");
    std::string line;
    std::string serial;

    if (!i) {
        return "0000000000000000";
    }

    while (getline(i, line)) {
        if (line.find("Serial") != std::string::npos) {
            serial = line.substr(line.length()- 16);
            return serial;
        }
    }
    return "00000000000000";
}