//
// Created by A547832 on 2017-06-15.
//

//TODO Move these to corresponding classes. Please.

#include "dbFunctions.h"

bool load_db_param(db_auth *params, std::string path = "/tempserver_remote/cpp/secrets") {
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

std::string sql_timestamp() {
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

bool get_remote_info(db_auth *auth, remote_info *rem) {
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

    remote_info rem_info;

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

bool save_status_message(board_parameters *board_param, db_auth *auth) {
  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement *prep_stmt;

    driver = get_driver_instance();
    con = driver->connect(auth->host, auth->user, auth->pwd);
    con->setSchema(auth->database);

    prep_stmt = con->prepareStatement("INSERT into remote_status_messages (cpu_idle, cpu_iowait, "
    "cpu_irq, cpu_nice, cpu_softirq, cpu_system, cpu_temp, cpu_user, "
    "disk_available, disk_size, disk_used, disk_used_percent, "
    "memory_buffers, memory_cache, memory_mem_free, memory_mem_total, "
    "memory_non_cabuf_mem, memory_swap, memory_total_used, "
    "network_avg_lat, network_max_lat, network_mdev_lat, network_min_lat, "
    "network_received, network_sent) "
    "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");

    prep_stmt->setDouble(1,board_param->cpu.idle);
    prep_stmt->setDouble(2,board_param->cpu.iowait);
    prep_stmt->setDouble(3,board_param->cpu.irq);
    prep_stmt->setDouble(4,board_param->cpu.nice);
    prep_stmt->setDouble(5,board_param->cpu.softirq);
    prep_stmt->setDouble(6,board_param->cpu.system);
    prep_stmt->setDouble(7,board_param->cpu.temp);
    prep_stmt->setDouble(8,board_param->cpu.user);
    prep_stmt->setDouble(9,board_param->disk.available);
    prep_stmt->setDouble(10,board_param->disk.size);
    prep_stmt->setDouble(11,board_param->disk.used);
    prep_stmt->setDouble(12,board_param->disk.used_p);
    prep_stmt->setDouble(13,board_param->memory.buffers);
    prep_stmt->setDouble(14,board_param->memory.cache);
    prep_stmt->setDouble(15,board_param->memory.mem_free);
    prep_stmt->setDouble(16,board_param->memory.mem_total);
    prep_stmt->setDouble(17,board_param->memory.non_cabuf_mem);
    prep_stmt->setDouble(18,board_param->memory.swap);
    prep_stmt->setDouble(19,board_param->memory.total_used);
    prep_stmt->setDouble(20,board_param->network.avg_ms);
    prep_stmt->setDouble(21,board_param->network.max_ms);
    prep_stmt->setDouble(22,board_param->network.mdev_ms);
    prep_stmt->setDouble(23,board_param->network.min_ms);
    prep_stmt->setInt(24,board_param->network.received);
    prep_stmt->setInt(25,board_param->network.sent);
    prep_stmt->executeQuery();
    delete(prep_stmt);
    delete(con);

  } catch (sql::SQLException &e) {
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return false;
  }
  return true;
}

std::vector<board_parameters> get_saved_status_messages(db_auth *auth) {
  std::string query = "SELECT * FROM remote_status_messages ORDER BY measurement_time DESC LIMIT 10";

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

    int res_len = res->rowsCount();
    int i = 0;

    std::vector<board_parameters> rows(res_len);

    while (res->next()) {
      rows[i].cpu.idle = res->getDouble("cpu_idle");
      rows[i].cpu.iowait = res->getDouble("cpu_iowait");
      rows[i].cpu.irq = res->getDouble("cpu_irq");
      rows[i].cpu.nice = res->getDouble("cpu_nice");
      rows[i].cpu.softirq = res->getDouble("cpu_softirq");
      rows[i].cpu.system = res->getDouble("cpu_system");
      rows[i].cpu.temp = res->getDouble("cpu_temp");
      rows[i].cpu.user = res->getDouble("cpu_user");
      rows[i].disk.available = res->getDouble("disk_available");
      rows[i].disk.size = res->getDouble("disk_size");
      rows[i].disk.used = res->getDouble("disk_used");
      rows[i].disk.used_p = res->getDouble("disk_used_percent");
      rows[i].memory.buffers = res->getDouble("memory_buffers");
      rows[i].memory.cache = res->getDouble("memory_cache");
      rows[i].memory.mem_free = res->getDouble("memory_mem_free");
      rows[i].memory.mem_total = res->getDouble("memory_mem_total");
      rows[i].memory.non_cabuf_mem = res->getDouble("memory_non_cabuf_mem");
      rows[i].memory.swap = res->getDouble("memory_swap");
      rows[i].memory.total_used = res->getDouble("memory_total_used");
      rows[i].network.avg_ms = res->getDouble("network_avg_lat");
      rows[i].network.max_ms = res->getDouble("network_max_lat");
      rows[i].network.mdev_ms = res->getDouble("network_mdev_lat");
      rows[i].network.min_ms = res->getDouble("network_min_lat");
      rows[i].network.received = res->getInt("network_received");
      rows[i].network.sent = res->getInt("network_sent");
      rows[i].timestamp = res->getString("measurement_time");
      rows[i].id = res->getInt("id");
      i++;
    }

    delete res;
    delete stmt;
    delete con;
    return rows;
  } catch (sql::SQLException &e) {
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
  }
  std::vector<board_parameters> a;
  return a;
}

bool remove_status_messages(db_auth *auth, board_vector measurements_to_remove) {
  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement *prep_stmt;

    driver = get_driver_instance();
    con = driver->connect(auth->host, auth->user, auth->pwd);
    con->setSchema(auth->database);

    prep_stmt = con->prepareStatement("DELETE FROM remote_status_messages WHERE id = (?) AND measurement_time = (?)");

    for (unsigned int i = 0; i < measurements_to_remove.size(); i++) {
      prep_stmt->setInt(1,measurements_to_remove[i].id);
      prep_stmt->setString(2,measurements_to_remove[i].timestamp);
      prep_stmt->execute();
    }

    delete(prep_stmt);
    delete(con);

  } catch (sql::SQLException &e) {
    std::cout << "# ERR: SQLException in " << __FILE__;
    std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    return false;
  }
  return true;
}
