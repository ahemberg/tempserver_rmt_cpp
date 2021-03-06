//
// Created by A547832 on 2017-06-20.
//

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <cstdio>
#include <memory>
#include <array>
#include <math.h>

#include "../structs.h"
#include "../nlohmann_json/json.hpp"

std::string exec(const char* cmd);
bool get_rpi_serial(std::string *s_no, bool set_null_if_fail = true);
bool get_cpu_temp(board_cpu *cpu, bool set_neg_if_fail = true);
bool get_cpu_load(board_cpu *cpu, bool set_neg_if_fail = true);
bool get_ram_info(board_memory *mem_info);
double convert_to_bytes(char unit, double value);
bool get_hdd_usage(board_disk_status *dsk);
void ping_server(board_network_status *net, const char* ping = "ping -c 5 8.8.8.8");
