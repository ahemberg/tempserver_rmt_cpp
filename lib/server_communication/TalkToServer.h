//
// Created by A547832 on 2017-06-05.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include "../nlohmann_json/json.hpp"
#include "../database/RemoteMainDB.h"

class TalkToServer : virtual public RemoteMainDB {
private:
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

protected:
  bool parse_server_response();

public:
  int server_response_code;
  std::string server_response_msg, encoded_post, raw_server_response = "";
  nlohmann::json server_message, server_saved_data;

  //TalkToServer(remote_info rem_info, temperature_vector temps_to_send);
  bool post_to_server();
  bool post_to_server(std::string post, std::string server_address = "");

  //std::string url_encode(const std::string &value);
  void url_encode(const std::string &value);

};
