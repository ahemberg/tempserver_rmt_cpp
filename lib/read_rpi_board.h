//
// Created by A547832 on 2017-06-20.
//

#ifndef TEMPSERVER_RMT_CPP_READ_RPI_BOARD_H
#define TEMPSERVER_RMT_CPP_READ_RPI_BOARD_H

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>

bool get_rpi_serial(std::string *s_no, bool set_null_if_fail = true);
bool get_cpu_temp(double *temp, bool set_neg_if_fail = true);

#endif //TEMPSERVER_RMT_CPP_READ_RPI_BOARD_H
