//
// Created by A547832 on 2017-07-04.
//

#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "RemoteMainDB.h"
#include "../get_board_status/GetBoardStatus.h"

class RemoteStatusDB : virtual public RemoteMainDB, public GetBoardStatus {
public:

  bool save_status_message();
  bool save_status_message(board_parameters *board_param);
  bool save_status_message(board_parameters *board_param, db_auth *auth);
  std::vector<RemoteStatusDB::board_parameters> get_saved_status_messages();
  std::vector<board_parameters> get_saved_status_messages(db_auth *auth);
  
  bool remove_status_messages(std::vector<RemoteStatusDB::board_parameters> measurements_to_remove);
  bool remove_status_messages(db_auth *auth, std::vector<RemoteStatusDB::board_parameters> measurements_to_remove);

protected:
private:
};
