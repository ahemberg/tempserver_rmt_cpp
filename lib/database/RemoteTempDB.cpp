//
// Created by A547832 on 2017-06-30.
//

#include "RemoteTempDB.h"

std::vector<RemoteTempDB::saved_temp> RemoteTempDB::get_saved_temperatures() {
  return RemoteTempDB::get_saved_temperatures(&sql_auth);
}

std::vector<RemoteTempDB::saved_temp> RemoteTempDB::get_saved_temperatures(RemoteTempDB::db_auth *auth) {

  std::string query = "SELECT * FROM saved_temp ORDER BY measurement_time DESC LIMIT 50";

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

    std::vector<RemoteTempDB::saved_temp> rows(res_len);

    while (res->next()) {
      rows[i].id = res->getInt("id");
      rows[i].timestamp = res->getString("measurement_time");
      rows[i].temp = res->getDouble("temp");
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
  std::vector<RemoteTempDB::saved_temp> a;
  return a;
}

bool RemoteTempDB::save_temp(double temperature) {
  return RemoteTempDB::save_temp(temperature, &sql_auth);
}

bool RemoteTempDB::save_temp(double temperature, RemoteTempDB::db_auth *auth) {

  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement *prep_stmt;

    driver = get_driver_instance();
    con = driver->connect(auth->host, auth->user, auth->pwd);
    con->setSchema(auth->database);

    prep_stmt = con->prepareStatement("INSERT into saved_temp (temp) VALUES (?)");

    prep_stmt->setDouble(1,temperature);
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

bool RemoteTempDB::remove_temps(RemoteTempDB::db_auth auth, std::vector<RemoteTempDB::saved_temp> temps_to_remove) {
  try {
    sql::Driver *driver;
    sql::Connection *con;
    sql::PreparedStatement *prep_stmt;

    driver = get_driver_instance();
    con = driver->connect(auth.host, auth.user, auth.pwd);
    con->setSchema(auth.database);

    prep_stmt = con->prepareStatement("DELETE FROM saved_temp WHERE id = (?) AND measurement_time = (?)");

    for (unsigned int i = 0; i < temps_to_remove.size(); i++) {
      prep_stmt->setInt(1,temps_to_remove[i].id);
      prep_stmt->setString(2,temps_to_remove[i].timestamp);
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
