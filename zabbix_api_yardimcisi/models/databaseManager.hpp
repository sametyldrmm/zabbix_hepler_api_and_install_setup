#ifndef DATABASEMANAGER_HPP
#define DATABASEMANAGER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <pqxx/pqxx>
#include <algorithm>
#include <string>
// daha önce bu struct tanımlandıysa tanımlanmasın
#ifndef DATABASE_CONFIG
#define DATABASE_CONFIG
    struct DatabaseConfig
    {
        std::string host;
        std::string port;
        std::string user;
        std::string password;
        std::string dbname;
    };
#endif


class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    void Connect();
    void Disconnect();
    bool ExecuteQuery(const std::string &query);
    void AddColumn(const std::string &tableName, const std::string &columnName, const std::string &dataType);
    void AddRow(const std::string &tableName, const std::vector<std::string> &values);
    std::string GetColumnValue(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue, const std::string &returnColumn);
    void CreateTable(const std::string &tableName, const std::vector<std::string> &columns);
    pqxx::result ExecuteSelectQuery(const std::string &query);
    std::vector<std::string> GetAllColumnValues(const std::string &tableName, const std::string &columnName);
    void UpdateRow(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue, const std::string &updateColumn, const std::string &updateValue);
    void DeleteRow(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue);
    void SelectRows(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue);
    void CountRows(const std::string &tableName);
    void SelectAllRows(const std::string &tableName);
    void DeleteAllRows(const std::string &tableName);
    void DropTable(const std::string &tableName);
    void TruncateTable(const std::string &tableName);
    void GetTableNames();
private:
    DatabaseConfig dbConfig;
    pqxx::connection *connection;

protected:
    DatabaseConfig ReadDatabaseConfigFromFile(const std::string &filename);
};

#endif