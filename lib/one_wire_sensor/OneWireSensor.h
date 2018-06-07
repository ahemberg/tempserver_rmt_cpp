//
// Created by A547832 on 2017-06-03.
//

/*
* Class for reading the one-wire sensor of the rpi
*/

#pragma once

#include <iostream>
#include <string.h>
#include <cstdio>
#include <dirent.h>
#include <unistd.h>
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
