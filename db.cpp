#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include "tables.cpp"

using namespace std;

error generateDbEntry(const char* name) {
    fstream dbFile;
    error err;
    err.msg = "";
    dbFile.open(DATABASE_RECORDS, ios::in | ios::out | ios::app);
    string databaseName;
    while (getline(dbFile, databaseName)) {
        if (strcmp(databaseName.c_str(), name) == 0) {
            err.msg = "Database '" + databaseName + "' already exists";
            return err;
        }
    }
    dbFile.clear();
    dbFile << name;
    dbFile << '\n';
    dbFile.close();
    return err;
}

bool createDB(database db) {
    error err = generateDbEntry(db.name.c_str());
    if (err.msg != "") {
        throwError(err);
        return false;
    } else {
        mkdir(db.name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    }
    return true;
}

database prepareDatabase(string name) {
    database db;
    db.name = name;
    db.tables = prepareTables();
    return db;
}

bool useDB(string name) {
    bool found = false;
    fstream dbFile;
    error err;
    dbFile.open(DATABASE_RECORDS, ios::in | ios::out | ios::app);
    string dbName;

    while (getline(dbFile, dbName)) {
        if (dbName == name) {
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }
    name = ROOT_URL + "/" + name + "/";
    chdir(name.c_str());
    // global: database in use
    currentDB = prepareDatabase(name);
    return true;
}
