//
// Created by A547832 on 2017-06-30.
//

#ifndef TEMPSERVER_RMT_CPP_REMOTETEMPDB_H
#define TEMPSERVER_RMT_CPP_REMOTETEMPDB_H

#include "RemoteMainDB.h"

class RemoteTempDB : virtual public RemoteMainDB {
public:

    struct saved_temp {
        int id;
        double temp;
        std::string timestamp;
    };

    std::vector<saved_temp> saved_temperatures; //Uninitialized!!

    RemoteTempDB() = default;

    std::vector<saved_temp> get_saved_temperatures(db_auth *auth);
    bool save_temp(double temperature, db_auth *auth);
    bool remove_temps(db_auth auth, std::vector<saved_temp> temps_to_remove);
protected:
private:

};


#endif //TEMPSERVER_RMT_CPP_REMOTETEMPDB_H
