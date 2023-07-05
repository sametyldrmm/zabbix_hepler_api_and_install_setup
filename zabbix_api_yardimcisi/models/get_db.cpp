#include <iostream>
#include <fstream>
#include <vector>
#include <pqxx/pqxx>
#include <algorithm>
#include <string>

struct DatabaseConfig {
    std::string host;
    std::string port;
    std::string user;
    std::string password;
    std::string dbname;
};

class TableData {
public:
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> rows;
    DatabaseConfig dbConfig ;
    void DbLoadTableData(const std::string& tableName) {
        try {
            dbConfig = ReadDatabaseConfigFromFile(std::string("./models/db_config").c_str());
            // Veritabanı bağlantısını oluştur
            std::string connString = "host=" + dbConfig.host + " port=" + dbConfig.port + " user=" + dbConfig.user + " password=" + dbConfig.password + " dbname=" + dbConfig.dbname;
            pqxx::connection conn(connString);

            if (conn.is_open()) {
                // Sorgu hazırlığı
                std::string query = "SELECT * FROM " + tableName;
                pqxx::work txn(conn);
                pqxx::result result = txn.exec(query);

                // Sütun başlıklarını al
                for (int i = 0; i < result.columns(); i++) {
                    columnNames.push_back(result.column_name(i));
                }

                // Sorgudan alınan değerleri vektöre ekle
                for (auto const& row : result) {
                    std::vector<std::string> rowData;
                    for (int i = 0; i < result.columns(); i++) {
                        rowData.push_back(row[i].c_str());
                    }
                    rows.push_back(rowData);
                }

                txn.commit();
                conn.disconnect();
            } else {
                std::cerr << "Veritabanı bağlantısı başarısız." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Hata: " << e.what() << std::endl;
        }
    }

    void DbPrintColumnNames() const {
        for (const auto& columnName : columnNames) {
            std::cout << columnName << "\t";
        }
        std::cout << std::endl;
    }

    void DbPrintTableData() const {
        for (const auto& row : rows) {
            for (const auto& field : row) {
                std::cout << field << "\t";
            }
            std::cout << std::endl;
        }
    }
DatabaseConfig ReadDatabaseConfigFromFile(const std::string& filename) {
    std::ifstream file(filename);
    DatabaseConfig config;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);

                if (key == "host") {
                    config.host = value;
                } else if (key == "port") {
                    config.port = value;
                } else if (key == "user") {
                    config.user = value;
                } else if (key == "password") {
                    config.password = value;
                } else if (key == "dbname") {
                    config.dbname = value;
                }
            }
        }
        file.close();
    } else {
        std::cerr << "Dosya açılamadı: " << filename << std::endl;
    }

    return config;
}
};



// int main() {
//     std::string tableName = "item";
//     std::string configFile = "db_config";

//     DatabaseConfig dbConfig = ReadDatabaseConfigFromFile(configFile);

//     TableData tableData;
//     tableData.LoadTableData(tableName, dbConfig);

//     std::cout << "Column Names:" << std::endl;
//     tableData.PrintColumnNames();

//     std::cout << "Table Data:" << std::endl;
//     tableData.PrintTableData();

//     return 0;
// }
