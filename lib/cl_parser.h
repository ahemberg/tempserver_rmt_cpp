//
// Created by A547832 on 2017-06-18.
//

#ifndef TEMPSERVER_RMT_CPP_CL_PARSER_H
#define TEMPSERVER_RMT_CPP_CL_PARSER_H

#include <iostream>
#include <unistd.h>
#include <cstring>

struct cl_opt {
    bool send_to_server = true,
            measure_temp = true,
            save_local = true,
            print = true,
            verbose = false;
};

void show_usage(std::string name);
bool parse_cl_opt(int c, char *v[], cl_opt* options);

#endif //TEMPSERVER_RMT_CPP_CL_PARSER_H
