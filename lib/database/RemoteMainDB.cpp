//
// Created by A547832 on 2017-06-30.
//

#include "RemoteMainDB.h"

std::string RemoteMainDB::sql_timestamp() {
  /*
  * Returns the current system timestamp in SQL Format as:
  * YYYY-MM-DD HH:MM:SS
  */

  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  std::string str(buffer);

  return str;
}

bool RemoteMainDB::load_db_param(std::string path) {
  return RemoteMainDB::load_db_param(&sql_auth, path);
}

bool RemoteMainDB::load_db_param(RemoteMainDB::db_auth *params, std::string path) {
  /*
  * Loads database authentication info from local secrets file. Params should point to a
  * db_auth struct. Secrets file has to be located in $HOME/tempserver_remote/cpp/
  *
  * Returns true if file was loaded properly, false otherwise.
  *
  */

  std::string homeDir = getenv("HOME");
  std::string filePath = path;
  nlohmann::json j;

  std::ifstream i(homeDir+filePath, std::ios::in);

  if (i) {
    try {
      i >> j;
      params->host = j["database_auth"]["host"];
      params->database = j["database_auth"]["database"];
      params->user = j["database_auth"]["user"];
      params->pwd = j["database_auth"]["password"];
    } catch (std::domain_error &e) {
      std::cout << "Bad format in secrets file!" << std::endl;
      return false;
    }
    return true;
  } else {
    std::cout << "Failed to load secrets file!" << std::endl;
    return false;
  }
}

bool RemoteMainDB::get_remote_info() {
  return RemoteMainDB::get_remote_info(&sql_auth, &remote);
}

bool RemoteMainDB::get_remote_info(remote_info *rem) {
  return RemoteMainDB::get_remote_info(&sql_auth, rem);
}

bool RemoteMainDB::get_remote_info(RemoteMainDB::db_auth *auth, RemoteMainDB::remote_info *rem) {
  /*
  * Gets remote information from the local remote database. Uses db_auth struct to
  * authenticate with the database. Populates a remote_info struct with remote information
  * from the database.
  *
  * Returns true if retrieval was successful, false otherwise
  */

  std::string query = "SELECT * from REMOTE_INFO LIMIT 1";

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    driver = get_driver_instance();
    con = driver->connect(auth->host, auth->user, auth->pwd);
    con->setSchema(auth->database);

    stmt = con->createStatement();
    res = stmt->executeQuery(query);

    RemoteMainDB::remote_info rem_info;

    while (res->next()) {
      rem->remote_id = res->getInt("remote_id");
      rem->sensor_directory = res->getString("sensor_directory");
      rem->server_address = res->getString("server_address");
      rem->sensor_serial = res->getString("sensor_serial");
    }

    delete res;
    delete stmt;
    delete con;

    return true;

  } catch (sql::SQLException &e) {
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
  }
  return false;

}
