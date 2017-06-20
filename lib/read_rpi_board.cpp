//
// Created by A547832 on 2017-06-20.
// Functions for reading RPI board info
//

#include "read_rpi_board.h"

bool get_rpi_serial(std::string *s_no, bool set_null_if_fail) {

    std::ifstream i("/proc/cpuinfo");
    std::string line;
    std::string serial;

    if (i) {
        while (getline(i, line)) {
            if (line.find("Serial") != std::string::npos) {
                *s_no = line.substr(line.length() - 16);
                return true;
            }
        }
        std::cerr << "Could not find serial in \"/proc/cpuinfo\"" << std::endl;
    }
    else {
        std::cerr << "Failed to read \"/proc/cpuinfo\"" << std::endl;
    }
    if (set_null_if_fail) {
        *s_no = "0000000000000000";
    }
    return false;
}

bool get_cpu_temp(double *temp, bool set_neg_if_fail) {

    std::ifstream i("/sys/class/thermal/thermal_zone0/temp");
    std::string line;

    if (i) {
        while (getline(i, line)) {
            if (line.length() == 5) {
                try {
                    *temp = std::stod(line) / 1000;
                    return true;
                } catch (std::invalid_argument &e) {
                    std::cerr << "Error Converting to double" << std::endl;
                    std::cerr << "Read:" << line << std::endl;
                    std::cerr << "e.what(): " << e.what() << std::endl;
                }
            }
        }
    } else {
        std::cerr << "Failed to read \"/sys/class/thermal/thermal_zone0/temp\"" << std::endl;
    }
    if (set_neg_if_fail) *temp = -1;
    return false;
}