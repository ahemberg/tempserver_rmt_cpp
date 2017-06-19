//
// Created by A547832 on 2017-06-05.
//

#ifndef TEMPSERVER_RMT_CPP_STRUCTS_H
#define TEMPSERVER_RMT_CPP_STRUCTS_H

#include <vector>
#include <string.h>

struct db_auth {
    std::string host;
    std::string database;
    std::string user;
    std::string pwd;
};

struct saved_temp {
    int id;
    double temp;
    std::string timestamp;
};

struct remote_info {
    std::string server_address;
    std::string sensor_directory;
    std::string sensor_serial;
    std::string board_serial;
    int remote_id;
};

typedef std::vector<saved_temp> temperature_vector;

#endif //TEMPSERVER_RMT_CPP_STRUCTS_H
