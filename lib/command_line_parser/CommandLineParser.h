/*
 * Sets and parses comman line options
 *
 * @author: Alexander Hemberg
 */

#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstring>

class CommandLineParser {
public:
  struct info_messages {
    const std::string reading_rem_info = "Reading remote info from database...",
          reading_sensor               = "Reading sensor...",
          sensor_read                  = "Sensor read! Temperature is ",
          saving_to_local              = "Saving temp to local storage...",
          temp_saved_local             = "Temp saved to local storage.",
          getting_local_stored         = "Getting locally stored measurements..",
          no_send_exit                 = "Nothing to send, exiting",
          data_to_send                 = "The following will be sent to the server:",
          sending                      = "Sending to server...",
          server_save_ok               = "Server successfully saved temperatures.",
          server_msg                   = "Server message: ",
          removing_local               = "Removing data saved on server from local storage..",
          removed                      = "Local data removed",
          local_storage_kept           = "Local storage has been kept.";
  };

  struct error_messages {
    const std::string error_reading_rem_info = "Failed to get remote info from database",
          error_reading_temp                 = "Unable to read temperature, Exiting.",
          error_storing_local                = "Unable to store to local database. Aborting",
          error_failed_server_contact        = "Failed to contact server. Local storage has been kept.",
          error_failed_server_save           = "Server failed to save temperatures",
          error_reading_board_serial         = "Failed to get rpi serial. Server will reject save!",
          error_unexpected_server_response   = "Unexpected format in server response. Local storage has been kept";
  };

  struct cl_opt {
    bool send_to_server = true,
         measure_temp = true,
         save_local = true,
         print = true,
         verbose = false;
  };

protected:
public:

  error_messages errors;
  info_messages info;
  cl_opt options;

  CommandLineParser() = default;
  bool parse_cl_opt_rt(int c, char **v);
  void show_usage_rt(std::string name);
};
