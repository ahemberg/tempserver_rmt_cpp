//
// Created by A547832 on 2017-06-20.
//

#ifndef TEMPSERVER_RMT_CPP_READ_RPI_BOARD_H
#define TEMPSERVER_RMT_CPP_READ_RPI_BOARD_H

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <cstdio>
#include <memory>
#include <array>

#include "structs.h"


std::string exec(const char* cmd);
bool get_rpi_serial(std::string *s_no, bool set_null_if_fail = true);
bool get_cpu_temp(double *temp, bool set_neg_if_fail = true);
bool get_ram_info(board_memory *mem_info);
double convert_to_bytes(char unit, double value);
bool get_hdd_usage(board_disk_status *dsk);


#endif //TEMPSERVER_RMT_CPP_READ_RPI_BOARD_H
