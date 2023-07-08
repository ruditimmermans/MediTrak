//
// Created by adam on 6/18/23.
//

#ifndef MEDICATIONTRACKER_DBMANAGER_H
#define MEDICATIONTRACKER_DBMANAGER_H

#include "../sqlite/sqlite3.h"

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <android/log.h>

using namespace std;

class DbManager {
private:
    /**
     * Path to the database file
     */
    string database_name;
    /**
     * Database reference
     */
    sqlite3* db{};

public:
    /**
     * Constructor for DbManager class
     * @param fileDescriptor Name of file containing database
     */
    DbManager(string fileDescriptor);

    /**
     * Opens database
     */
    void open();

    /**
     * Closes database
     */
    void close();

    /**
     * Gets all tables in database
     * @return A pointer to a string to hold table names
     */
    string* getTables();

    /**
     * Retrieves all values from a database table
     * @param table The table from which to retrieve results
     * @return A vector containing all values in "table".
     *      Each element of the vector is a map whose key is the column name
     *      and value is the column value.
     */
    vector<map<string, string>> readAllValuesInTable(const string& table);

    /**
     * Retrieves all data from all tables in the database
     * @return A map to contain all data in the database.
     *      Each key should be a table name.
     *      Each value should be a pointer to vector a maps whose keys are column names
     *      and values are column values
     */
    map<string, vector<map<string, string>>*> getAllRowFromAllTables();
};


#endif //MEDICATIONTRACKER_DBMANAGER_H
