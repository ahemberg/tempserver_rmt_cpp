//
// Created by A547832 on 2017-06-21.
//

#include "SendTempToServer.h"

SendTempToServer::SendTempToServer(remote_info rem_info, temperature_vector temps_to_send){
    local_temps = temps_to_send;
    remote_data = rem_info;

    //Set server message
    generate_server_temperature_message();

    //Encode server message
    url_encode(server_message.dump());
}

void SendTempToServer::generate_server_temperature_message() {
    nlohmann::json temperatures;

    for (unsigned int i = 0; i<local_temps.size(); i++) {
        nlohmann::json j3;
        j3.push_back(nlohmann::json::object_t::value_type("temp",local_temps[i].temp));
        j3 += nlohmann::json::object_t::value_type("measurement_time",local_temps[i].timestamp);
        j3 += nlohmann::json::object_t::value_type("id",local_temps[i].id);

        temperatures.push_back(j3);
    }

    server_message["temperatures"] = temperatures;
    server_message["remote_id"] = remote_data.remote_id;
    server_message["remote_serial"] = remote_data.board_serial;
}

//TODO: SPLIT THIS SO THAT HIS PARSES, AND DATA IS HANDLED IN SUB_CLASS
void SendTempToServer::parse_saved_temperatures() {

    auto server_response = nlohmann::json::parse(raw_server_response);

    server_response_code = server_response["status"];
    server_response_msg = server_response["msg"];
    server_saved_data = server_response["saved_data"];

    saved_temp t;
    for (auto& element : server_response["saved_data"]) {
        t.timestamp = element["measurement_time"];
        t.id = element["id"];
        t.temp = element["temp"];
        temps_saved_on_server.push_back(t);
    }
}