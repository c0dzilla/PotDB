#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include "tables.cpp"

using namespace std;

error generateDbEntry(const char *name) {
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

error deleteDbEntry(const char *name) {
    error err;
    err.msg = "";
    ifstream dbFile;
    dbFile.open(DATABASE_RECORDS, ios::in | ios::out | ios::app);
    ofstream temp;
    temp.open("temp.txt");
    string databaseName;
    bool flag = false;
    while (getline(dbFile, databaseName)) {
        if (databaseName != name) {
            temp << databaseName << endl;
        } else {
            flag = true;
        }
    }
    temp.close();
    dbFile.close();
    remove(DATABASE_RECORDS);
    rename("temp.txt", DATABASE_RECORDS);
    if (!flag){
        return err;
    }else {
        err.msg = "NO DATABASE FOUND";
        return err;
    }
}

void createDB(database db) {
    error err = generateDbEntry(db.name.c_str());
    if (err.msg != "") {
        throwError(err);
    } else {
        mkdir(db.name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    }
}

database prepareDatabase(string name) {
    database db;
    db.name = name;
    db.tables = prepareTables();
    return db;
}

void useDB(string name) {
    name = ROOT_URL + "/" + name + "/";
    chdir(name.c_str());
    // global: database in use
    currentDB = prepareDatabase(name);
}

void dropdb(database db) {
    error err = deleteDbEntry(db.name.c_str());
    if (err.msg != "") {
        throwError(err);
    } else {
        rmdir(db.name.c_str());
    }
}