//
// Created by A547832 on 2017-06-15.
//

#include "dbFunctions.h"

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

std::vector<saved_temp> get_saved_temperatures(db_auth *auth) {

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

        std::vector<saved_temp> rows(res_len);

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
}

bool save_temp(double temperature, db_auth *auth) {

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

bool remove_temps(db_auth auth, std::vector<saved_temp> temps_to_remove) {

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::PreparedStatement *prep_stmt;

        driver = get_driver_instance();
        con = driver->connect(auth.host, auth.user, auth.pwd);
        con->setSchema(auth.database);

        prep_stmt = con->prepareStatement("DELETE FROM saved_temp WHERE id = (?) AND measurement_time = (?)");

        for (int i = 0; i < temps_to_remove.size(); i++) {
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