//
// Created by A547832 on 2017-06-15.
//

// TODO: A class of this?

#ifndef TEMPSERVER_RMT_CPP_DBFUNCTIONS_H
#define TEMPSERVER_RMT_CPP_DBFUNCTIONS_H

//Dependencies
#include <iostream>
#include <string.h>
#include <vector>
#include <time.h>



//MySQL Includes
#include <cppconn/driver.h>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

//Misc
#include "structs.h"

std::string sql_timestamp();
bool get_remote_info(db_auth *auth, remote_info *rem);
std::vector<saved_temp> get_saved_temperatures(db_auth *auth);
bool save_temp(double temperature, db_auth *auth);
bool remove_temps(db_auth auth, std::vector<saved_temp> temps_to_remove);

#endif //TEMPSERVER_RMT_CPP_DBFUNCTIONS_H
