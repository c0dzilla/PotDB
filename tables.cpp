#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include "constants.cpp"
#include "errors.cpp"

using namespace std;

error generateTableEntry(const char* name) {
    fstream tablesRecord;
    error err;
    err.msg = "";
    tablesRecord.open(TABLE_RECORDS, ios::in | ios::out | ios::trunc);
    if (!tablesRecord) {
       std::cout << "Failed to open file\n";
       exit(1);
   }
    string tableName;
    while (getline(tablesRecord, tableName)) {
        if (strcmp(tableName.c_str(), name) == 0) {
            err.msg = "DUPLICATE TABLE";
            return err;
        }
    }
    tablesRecord.clear();
    tablesRecord << name << '\n';
    tablesRecord.close();
    return err;
}

void createTableRecord(string path, table t) {
    string file = path + SCHEMA;
    ofstream tablesFile;
    tablesFile.open(file.c_str());
    for (attritr = t.attributes.begin(); attritr != t.attributes.end(); attritr++) {
        tablesFile << attritr->first << '\n';
    }
    if (t.primaryKey != "") {
        tablesFile << ".\n" << t.primaryKey;
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

map<string, table> prepareTables() {
    map<string, table> tables;
    fstream tablesRecord;
    tablesRecord.open(TABLE_RECORDS, ios::in | ios::out | ios::trunc);
    string tableName;
    while (getline(tablesRecord, tableName)) {
        table t;
        t.name = tableName;
        string path = t.name + "/";
        t.attributes = prepareAttributes(path);
        tables[tableName] = t;
    }
    tablesRecord.close();
    return tables;
}

map<string, attribute> prepareAttributes(string path) {
    string file = path + SCHEMA;
    fstream tableFile;
    map<string, attribute> attributes;
    tableFile.open(TABLE_RECORDS, ios::in | ios::out | ios::trunc);
    string line;
    while (getline(tableFile, line)) {
        // primary key field
        if (line == ".") {
            break;
        }
        attribute attr;
        attr.name = line;
        attributes[attr.name] = attr;
    }
    tableFile.close();
    return attributes;
}

