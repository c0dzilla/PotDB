#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include "constants.cpp"
#include "errors.cpp"

using namespace std;

void generateDbEntry(const char* name) {
    fstream dbFile;
    dbFile.open(DATABASE_RECORDS, ios::in | ios::out | ios::app);
    string databaseName;
    while (getline(dbFile, databaseName)) {
        if (strcmp(databaseName.c_str(), name) == 0) {
            error err;
            err.msg = "DUPLICATE DATABASE";
            return throwError(err);
        }
    }
    dbFile.clear();
    dbFile<<name;
    dbFile<<'\n';
    dbFile.close();
}

void createDB(database db) {
    generateDbEntry(db.name.c_str());
    mkdir(db.name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
}