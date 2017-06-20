//
// Created by A547832 on 2017-06-03.
//

/*
 * Class for reading the one-wire sensor of the rpi
 */

#ifndef TEMPSERVER_RMT_CPP_ONEWIRESENSOR_H
#define TEMPSERVER_RMT_CPP_ONEWIRESENSOR_H

#include <iostream>
#include <string.h>
//#include <stdio.h> TODO:REMOVE
#include <dirent.h>
#include <fcntl.h>


class OneWireSensor
{
private:

public:
    std::string sensor_serial;
    double sensor_temp;
    void get_sensor_serial(std::string sensor_path);
    void read_temp(std::string sensor_path, std::string sensor_sn_str);
};

#endif //TEMPSERVER_RMT_CPP_ONEWIRESENSOR_H
