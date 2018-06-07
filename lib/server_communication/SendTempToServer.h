//
// Created by A547832 on 2017-06-21.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "TalkToServer.h"
#include "../nlohmann_json/json.hpp"
#include "../database/RemoteTempDB.h"

class SendTempToServer : public TalkToServer, public RemoteTempDB {
private:
  //    void generate_server_temperature_message();
public:
  void generate_server_temperature_message();
  std::vector<SendTempToServer::saved_temp> local_temps, temps_saved_on_server;
  //temperature_vector local_temps, temps_saved_on_server;
  SendTempToServer() = default;
  //SendTempToServer(std::string secrets_path  = "/tempserver_remote/cpp/secrets");
  bool parse_saved_temperatures();
};
