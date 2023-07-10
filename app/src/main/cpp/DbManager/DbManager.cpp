//
// Created by adam on 6/18/23.
//

#include "DbManager.h"

#include <utility>

DbManager::DbManager(string fileDescriptor) {
    database_name = std::move(fileDescriptor);
    openDb();
}

DbManager::~DbManager() {
    closeDb();
}

void DbManager::openDb() { sqlite3_open(database_name.c_str(), &db); }
void DbManager::closeDb() { sqlite3_close(db); }

vector<string> DbManager::getTables() {
    int rc;
    sqlite3_stmt *stmt = nullptr;
    string query = "SELECT name FROM sqlite_schema WHERE type ='table' AND name NOT LIKE 'sqlite_%';";
    vector<string> tables;

    rc = sqlite3_prepare(db, query.c_str(), -1, &stmt, nullptr);

    sqlite3_step(stmt);

    try {
        if (rc != SQLITE_OK) {
            sqlite3_finalize(stmt);

            string err = "Error reading from SQLite database.";

            throw runtime_error(err);
        }

        while (sqlite3_column_text(stmt, 0)) {
            string colText;

            if (sqlite3_column_text(stmt, 0) != nullptr) {
                colText = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            }

            tables.push_back(colText);

            sqlite3_step(stmt);
        }
    } catch (runtime_error& error) {
        cerr << error.what();

        exit(1);
    }

    sqlite3_finalize(stmt);

    return tables;
}

vector<map<string, string>> DbManager::readAllValuesInTable(const string& table) {
    sqlite3_stmt *stmt = nullptr;
    string query = "SELECT * FROM " + table + ";";
    vector<map<string, string>> results;
    int rc;

    rc = sqlite3_prepare(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);

    try {
        if (rc != SQLITE_OK) {
            sqlite3_finalize(stmt);

            throw exception();
        }

        while (sqlite3_column_text(stmt, 0)) {
            map<string, string> m = map<string, string>();

            for (int i = 0; i < sqlite3_column_count(stmt); i++) {
                string colText;

                if (sqlite3_column_text(stmt, i) != nullptr) {
                    colText = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                }

                m.insert(
                        {
                                string(sqlite3_column_name(stmt, i)),
                                string(colText)
                        }
                );
            }

            results.push_back(m);
            sqlite3_step(stmt);
        }
    } catch (string& error) {
        cerr << "SQLITE READ ERROR | Return Code: " << rc << endl;

        exit(1);
    }

    sqlite3_finalize(stmt);

    return results;
}

map<string, vector<map<string, string>>> DbManager::getAllRowFromAllTables() {
    map<string, vector<map<string, string>>> tableData;
    vector<string> tables = getTables();

    for (const string& tbl : tables) {
        tableData.insert({ tbl, readAllValuesInTable(tbl) });
    }

    return tableData;
}

void DbManager::exportData(string exportFilePath) {
    time_t date = time(nullptr);
    tm* now = localtime(&date);
    map<string, vector<map<string, string>>> data = getAllRowFromAllTables();
    string fileName = "meditrak_"
            + to_string(now->tm_year) + "_"
            + to_string(now->tm_mon) + "_"
            + to_string(now->tm_mday) + ".json";
    stringstream fileData;
    string outData;

    int fd = open(exportFilePath.append(fileName).c_str(), O_CREAT, S_IRUSR);

    fileData << "{";

    // write json formatted data here

    fileData << "}";

    fileData >> outData;

    write(fd, fileName.c_str(), size_t(outData.c_str()));

    close(fd);
    delete now;
}
