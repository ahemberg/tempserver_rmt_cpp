//
// Created by A547832 on 2017-06-15.
//

// TODO: A class of this?

#pragma once

//Dependencies
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>

#include "../nlohmann_json/json.hpp"

//MySQL Includes
#include <cppconn/driver.h>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

//Misc
#include "../structs.h"

bool load_db_param(db_auth *params);
std::string sql_timestamp();
bool get_remote_info(db_auth *auth, remote_info *rem);
std::vector<saved_temp> get_saved_temperatures(db_auth *auth);
bool save_temp(double temperature, db_auth *auth);
bool remove_temps(db_auth auth, std::vector<saved_temp> temps_to_remove);
bool save_status_message(board_parameters *board_param, db_auth *auth);
std::vector<board_parameters> get_saved_status_messages(db_auth *auth);
bool remove_status_messages(db_auth *auth, board_vector measurements_to_remove);
