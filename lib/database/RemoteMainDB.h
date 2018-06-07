//
// Created by A547832 on 2017-06-30.
//

#pragma once

#include <iostream>
#include <string>
#include <fstream>
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

class RemoteMainDB {
public:

  struct db_auth {
    std::string host;
    std::string database;
    std::string user;
    std::string pwd;
  };

  struct remote_info {
    std::string server_address;
    std::string sensor_directory;
    std::string sensor_serial;
    std::string board_serial;
    int remote_id;
  };

  db_auth sql_auth; //UNIN!!
  remote_info remote; //UNIN!!

  RemoteMainDB() = default;

  std::string sql_timestamp();

  //Overloaded Load db Param. Version one loads param to external variable, other sets class local.
  bool load_db_param(db_auth *params, std::string path = "/tempserver_remote/cpp/secrets");
  bool load_db_param(std::string path = "/tempserver_remote/cpp/secrets");

  //Overloaded get remote info
  bool get_remote_info(); //Uses local sql_auth, stores in local remote
  bool get_remote_info(remote_info *rem); //Uses local sql_auth, stores to external rem
  bool get_remote_info(db_auth *auth, remote_info *rem); //Uses external auth, stores to external rem

protected:
private:
};
