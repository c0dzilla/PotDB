#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include "constants.cpp"
#include "errors.cpp"

using namespace std;

error generateTableEntry(const char* name) {
    fstream tablesFile;
    error err;
    err.msg = "";
    tablesFile.open(TABLE_RECORDS, ios::in | ios::out | ios::trunc);
    if (!tablesFile) {
       std::cout << "Failed to open file\n";
       exit(1);
   }
    string tableName;
    while (getline(tablesFile, tableName)) {
        if (strcmp(tableName.c_str(), name) == 0) {
            err.msg = "DUPLICATE TABLE";
            return err;
        }
    }
    tablesFile.clear();
    tablesFile << name << '\n';
    tablesFile.close();
    return err;
}

void createTableRecord(string path, table t) {
    string file = path + t.name;
    ofstream tablesFile;
    tablesFile.open(file.c_str());
    tablesFile << t.name << '\n';
    for (attritr = t.attributes.begin(); attritr != t.attributes.end(); attritr++) {
        tablesFile << attritr->first << '\n';
    }
    tablesFile.close();
}

void createTable(table t) {
    error err = generateTableEntry(t.name.c_str());
    if (strcmp(err.msg.c_str(), "") != 0) {
        throwError(err);
        return;
    }
    mkdir(t.name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    string path = t.name + "/";
    createTableRecord(path, t);
}
