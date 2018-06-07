//
// Created by A547832 on 2017-07-03.
//

#include "CommandLineParser.h"

void CommandLineParser::show_usage_rt(std::string name) {
  std::cerr << "Usage: " << name << " <option(s)>\n"
            << "Options:\n"
            << "-h \t\tShow this help message\n"
            << "-s [option]\tControl how the temperature is read and saved.\n"
            << "\t\tlocal  - Measure temp and save locally. Do not send to server\n"
            << "\t\tremote - Measure temp and send to server, do not save locally if server call fails,\n"
            << "\t\t\t does not send any previously saved measurements\n"
            << "\t\tno-save - Only measure temp, do not save or send. Prints to stdout\n"
            << "\t\tsend-only - Sends saved temperatures, does not measure temp\n"
            << "-v \t\tVerbose printing to stdout\n"
            << "-q \t\tPrints nothing (overrides -v)\n"
            << std::endl;
}

bool CommandLineParser::parse_cl_opt_rt(int c, char **v) {
  int opt;
  while ((opt = getopt(c, v, "s:v:h:q")) != -1) {
    switch (opt) {
      case 'h':
        show_usage_rt(v[0]);
        return false;
      case 's':
        if (!strcmp(optarg, "local")) {
          options.send_to_server = false;
        } else if (!strcmp(optarg, "remote")) {
          options.save_local = false;
        } else if (!strcmp(optarg, "no-save")) {
          options.send_to_server = false;
          options.save_local = false;
        } else if (!strcmp(optarg, "send-only")) {
          options.measure_temp = false;
        } else {
          std::cout << "Invalid option: " << optarg << std::endl;
          show_usage_rt(v[0]);
          return false;
        }
        break;
      case 'v':
        options.print = true;
        options.verbose = true;
        break;
      case 'q':
        options.print = false;
        break;
      default:
        show_usage_rt(v[0]);
        return false;
      }
    }
    return true;
}
