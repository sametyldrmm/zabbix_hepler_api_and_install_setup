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
    DatabaseManager() 
    {
        dbConfig = ReadDatabaseConfigFromFile(std::string("./models/db_config").c_str());
        Connect();
    }

    ~DatabaseManager()
    {
        Disconnect();
    }

    void Connect()
    {
        std::string connString = "host=" + dbConfig.host + " port=" + dbConfig.port + " user=" + dbConfig.user + " password=" + dbConfig.password + " dbname=" + dbConfig.dbname;
        connection = new pqxx::connection(connString);

        if (connection->is_open())
        {
            std::cout << "Veritabanına bağlanıldı." << std::endl;
        }
        else
        {
            std::cerr << "Veritabanı bağlantısı başarısız." << std::endl;
        }
    }

    void Disconnect()
    {
        if (connection && connection->is_open())
        {
            connection->disconnect();
            delete connection;
            connection = nullptr;
            std::cout << "Veritabanı bağlantısı kapatıldı." << std::endl;
        }
    }

    bool ExecuteQuery(const std::string &query)
    {
        if (!connection || !connection->is_open())
        {
            std::cerr << "Veritabanı bağlantısı bulunamadı." << std::endl;
            return false;
        }

        try
        {
            pqxx::work txn(*connection);
            txn.exec(query);
            txn.commit();
            std::cout << "Sorgu başarıyla gerçekleştirildi." << std::endl;
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Hata: " << e.what() << std::endl;
            return false;
        }
    }

    void AddColumn(const std::string &tableName, const std::string &columnName, const std::string &dataType)
    {
        std::string query = "ALTER TABLE " + tableName + " ADD COLUMN " + columnName + " " + dataType;
        ExecuteQuery(query);
    }

    void AddRow(const std::string &tableName, const std::vector<std::string> &values)
    {
        std::string query = "INSERT INTO " + tableName + " VALUES (";
        for (const auto &value : values)
        {
            query += "'" + value + "',";
        }
        query.pop_back(); // Remove the last comma
        query += ")";
        ExecuteQuery(query);
    }

    std::string GetColumnValue(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue, const std::string &returnColumn)
    {
        std::string query = "SELECT " + returnColumn + " FROM " + tableName + " WHERE " + searchColumn + " = '" + searchValue + "'";
        pqxx::result result = ExecuteSelectQuery(query);

        if (!result.empty())
        {
            return result[0][0].c_str();
        }
        else
        {
            return "";
        }
    }

    void CreateTable(const std::string &tableName, const std::vector<std::string> &columns)
    {
        std::string query = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
        for (const auto &column : columns)
        {
            query += column + ",";
        }
        query.pop_back(); // Remove the last comma
        query += ")";
        ExecuteQuery(query);
    }

    pqxx::result ExecuteSelectQuery(const std::string &query)
    {
        if (!connection || !connection->is_open())
        {
            std::cerr << "Veritabanı bağlantısı bulunamadı." << std::endl;
            return pqxx::result();
        }

        try
        {
            pqxx::work txn(*connection);
            pqxx::result result = txn.exec(query);
            txn.commit();
            std::cout << "Sorgu başarıyla gerçekleştirildi." << std::endl;
            return result;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Hata: " << e.what() << std::endl;
            return pqxx::result();
        }
    }

    std::vector<std::string> GetAllColumnValues(const std::string &tableName, const std::string &columnName)
    {
        std::vector<std::string> values;

        std::string query = "SELECT " + columnName + " FROM " + tableName;
        pqxx::result result = ExecuteSelectQuery(query);

        for (const auto &row : result)
        {
            values.push_back(row[0].c_str());
        }

        return values;
    }

    void UpdateRow(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue, const std::string &updateColumn, const std::string &updateValue)
    {
        std::string query = "UPDATE " + tableName + " SET " + updateColumn + " = '" + updateValue + "' WHERE " + searchColumn + " = '" + searchValue + "'";
        ExecuteQuery(query);
    }

    void DeleteRow(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue)
    {
        std::string query = "DELETE FROM " + tableName + " WHERE " + searchColumn + " = '" + searchValue + "'";
        ExecuteQuery(query);
    }

    void SelectRows(const std::string &tableName, const std::string &searchColumn, const std::string &searchValue)
    {
        std::string query = "SELECT * FROM " + tableName + " WHERE " + searchColumn + " = '" + searchValue + "'";
        pqxx::result result = ExecuteSelectQuery(query);

        for (const auto &row : result)
        {
            for (const auto &field : row)
            {
                std::cout << field.c_str() << "\t";
            }
            std::cout << std::endl;
        }
    }

    void CountRows(const std::string &tableName)
    {
        std::string query = "SELECT COUNT(*) FROM " + tableName;
        pqxx::result result = ExecuteSelectQuery(query);

        if (!result.empty())
        {
            std::cout << "Row count: " << result[0][0].c_str() << std::endl;
        }
    }

    void SelectAllRows(const std::string &tableName)
    {
        std::string query = "SELECT * FROM " + tableName;
        pqxx::result result = ExecuteSelectQuery(query);

        for (const auto &row : result)
        {
            for (const auto &field : row)
            {
                std::cout << field.c_str() << "\t";
            }
            std::cout << std::endl;
        }
    }

    void DeleteAllRows(const std::string &tableName)
    {
        std::string query = "DELETE FROM " + tableName;
        ExecuteQuery(query);
    }

    void DropTable(const std::string &tableName)
    {
        std::string query = "DROP TABLE IF EXISTS " + tableName;
        ExecuteQuery(query);
    }

    void TruncateTable(const std::string &tableName)
    {
        std::string query = "TRUNCATE TABLE " + tableName;
        ExecuteQuery(query);
    }

    void GetTableNames()
    {
        std::string query = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'";
        pqxx::result result = ExecuteSelectQuery(query);

        std::cout << "Table Names:" << std::endl;
        for (const auto &row : result)
        {
            std::cout << row[0].c_str() << std::endl;
        }
    }

private:
    DatabaseConfig dbConfig;
    pqxx::connection *connection;

protected:
    DatabaseConfig ReadDatabaseConfigFromFile(const std::string &filename)
    {
        std::ifstream file(filename);
        DatabaseConfig config;

        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
                size_t pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    if (key == "host")
                    {
                        config.host = value;
                    }
                    else if (key == "port")
                    {
                        config.port = value;
                    }
                    else if (key == "user")
                    {
                        config.user = value;
                    }
                    else if (key == "password")
                    {
                        config.password = value;
                    }
                    else if (key == "dbname")
                    {
                        config.dbname = value;
                    }
                }
            }
            file.close();
        }
        else
        {
            std::cerr << "Dosya açılamadı: " << filename << std::endl;
        }

        return config;
    }
};


