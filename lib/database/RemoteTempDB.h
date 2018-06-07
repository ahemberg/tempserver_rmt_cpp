//
// Created by A547832 on 2017-06-30.
//

#pragma once

#include "RemoteMainDB.h"

class RemoteTempDB : virtual public RemoteMainDB {
public:

  struct saved_temp {
    int id;
    double temp;
    std::string timestamp;
  };

  std::vector<RemoteTempDB::saved_temp> saved_temperatures; //Uninitialized!!

  RemoteTempDB() = default;

  std::vector<RemoteTempDB::saved_temp> get_saved_temperatures();
  std::vector<RemoteTempDB::saved_temp> get_saved_temperatures(RemoteTempDB::db_auth *auth);

  bool save_temp(double temperature);
  bool save_temp(double temperature, db_auth *auth);
  bool remove_temps(db_auth auth, std::vector<RemoteTempDB::saved_temp> temps_to_remove);
protected:
private:
};
