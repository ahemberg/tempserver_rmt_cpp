//
// Created by A547832 on 2017-06-19.
//

#ifndef TEMPSERVER_RMT_CPP_MESSAGES_ENG_H
#define TEMPSERVER_RMT_CPP_MESSAGES_ENG_H

#include <string>

struct output_messages {

    //INFO
    const std::string reading_rem_info =            "Reading remote info from database...",
                      reading_sensor =              "Reading sensor...",
                      sensor_read =                 "Sensor read! Temperature is ",
                      saving_to_local =             "Saving temp to local storage...",
                      temp_saved_local =            "Temp saved to local storage.",
                      getting_local_stored =        "Getting locally stored measurements..",
                      no_send_exit =                "Nothing to send, exiting",
                      data_to_send =                "The following will be sent to the server:",
                      sending =                     "Sending to server...",
                      server_save_ok =              "Server successfully saved temperatures.",
                      server_msg =                  "Server message: ",
                      removing_local =              "Removing data saved on server from local storage..",
                      removed =                     "Local data removed",
                      local_storage_kept =          "Local storage has been kept.";

    //ERROR MESSAGES
    const std::string error_reading_rem_info =      "Failed to get remote info from database",
                      error_reading_temp =          "Unable to read temperature, Exiting.",
                      error_storing_local =         "Unable to store to local database. Aborting",
                      error_failed_server_contact = "Failed to contact server. Local storage has been kept.",
                      error_failed_server_save =    "Server failed to save temperatures";
};

#endif //TEMPSERVER_RMT_CPP_MESSAGES_ENG_H
