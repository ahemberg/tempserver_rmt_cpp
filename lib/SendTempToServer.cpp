//
// Created by A547832 on 2017-06-21.
//

#include "SendTempToServer.h"
/*
//SendTempToServer::SendTempToServer(SendTempToServer::remote_info rem_info, std::vector<SendTempToServer::saved_temp> temps_to_send, std::string path = "/tempserver_remote/cpp/secrets"){
SendTempToServer::SendTempToServer(std::string secrets_path){
    //load_db_param(&sql_auth, secrets_path);
    //get_remote_info(&sql_auth, &remote_data);
    //local_temps = temps_to_send;

    //Set server message
    //generate_server_temperature_message();

    //Encode server message
    //url_encode(server_message.dump());
}
 */

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

//TODO Error handling
bool SendTempToServer::parse_saved_temperatures() {

    if (!parse_server_response()) return false; //GETS parameters
    saved_temp t;
    for (auto& element : server_saved_data) {
        t.timestamp = element["measurement_time"];
        t.id = element["id"];
        t.temp = element["temp"];
        temps_saved_on_server.push_back(t);
    }
    return true;
}