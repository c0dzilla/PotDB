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
    tablesRecord.open(TABLE_RECORDS, ios::in | ios::out | ios::app);
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
    map<string, attribute>::iterator attritr;
    for (attritr = t.attributes.begin(); attritr != t.attributes.end(); attritr++) {
        tablesFile << attritr->first << '\n';
    }
    if (t.primaryKey != "") {
        tablesFile << ".\n" << t.primaryKey;
    }
    tablesFile.close();
}

void createTable(table t) {
    error err;
    // primary key enforced(atleast for now)
    if (t.primaryKey == "") {
        err.msg = "NO PRIMARY KEY SPECIFIED";
        return;
    }
    err = generateTableEntry(t.name.c_str());
    if (err.msg != "") {
        throwError(err);
        return;
    }
    mkdir(t.name.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    string path = t.name + "/";
    createTableRecord(path, t);
}

map<string, attribute> prepareAttributes(table& t, string path) {
    string file = path + SCHEMA;
    fstream tableFile;
    map<string, attribute> attributes;
    tableFile.open(TABLE_RECORDS, ios::in | ios::out | ios::app);
    string line;
    while (getline(tableFile, line)) {
        // primary key field
        if (line == ".") {
            getline(tableFile, line);
            t.primaryKey = line;
            break;
        }
        attribute attr;
        attr.name = line;
        attributes[attr.name] = attr;
    }

    tableFile.close();
    return attributes;
}

map<string, table> prepareTables() {
    map<string, table> tables;
    fstream tablesRecord;
    tablesRecord.open(TABLE_RECORDS, ios::in | ios::out | ios::app);
    string tableName;
    while (getline(tablesRecord, tableName)) {
        table t;
        t.name = tableName;
        string path = t.name + "/";
        t.attributes = prepareAttributes(t, path);
        tables[tableName] = t;
    }
    tablesRecord.close();
    return tables;
}

vector<map<string, string>> insert(table t, vector<map<string, string>> rows) {
    error err;
    // check table existence
    if (currentDB.tables.find(t.name) == currentDB.tables.end()) {
        err.msg = "TABLE DOES NOT EXIST";
        throwError(err);
        vector<map<string, string>> r;
        return r;
    }
    // for each row
    for (auto itr = rows.begin(); itr != rows.end(); itr++) {
        map<string, string> row = *itr;
        bool valid = true;
        // check for invalid attribute
        for (auto attr = row.begin(); attr != row.end(); attr++) {
            if (t.attributes.find(attr->first) == t.attributes.end()) {
                err.msg = "Attribute " + attr->first + " does not exist";
                throwError(err);
                rows.erase(itr);
                valid = false;
                break;
            }
        }
        if (!valid) {
            continue;
        }
        // check for violation of not null constraint
        for (auto attr = t.attributes.begin(); attr != t.attributes.end(); attr++) {
            if (attr->second.nullAllowed == false && row.find(attr->first) == row.end()) {
                err.msg = "Attribute " + attr->first + " cannot be null";
                throwError(err);
            }
        }

        // write to file
        fstream record;
        string file = t.name + "/" + row[t.primaryKey];
        record.open(file, ios::in | ios::out | ios::app);
        for (auto attr = t.attributes.begin(); attr != t.attributes.end(); attr++) {
            string element = "";
            if (row.find(attr->first) != row.end()) {
                element = row[attr->first];
            }
            record << element << "\n";
        }
        
        record.close();
    }
}
