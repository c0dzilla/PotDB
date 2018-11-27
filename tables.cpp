#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <dirent.h>
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
            err.msg = "Table " + tableName + " already exists";
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
        attribute attr = attritr->second;
        tablesFile << attr.name << '\n';
        if (attr.nullAllowed == true) {
            tablesFile << "+" << '\n';
        } else {
            tablesFile << "-" << '\n';
        }
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
        err.msg = "No primary key specified";
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
    currentDB.tables[t.name] = t;
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
        getline(tableFile, line);
        if (line == "+") {
            attr.nullAllowed = true;
        } else {
            attr.nullAllowed = false;
        }
        attributes[attr.name] = attr;
    }

    tableFile.close();
    return attributes;
}

map<string, bool> generateIndex(string path) {
    DIR *dir = opendir(path.c_str());
    map<string, bool> index;
    struct dirent *ent;
    
    if (dir) {
        while (ent = readdir(dir)) {
            index[ent->d_name] = true;
        }
    }

    return index;
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
        t.index = generateIndex(path);
        tables[tableName] = t;
    }
    tablesRecord.close();
    return tables;
}

bool checkKeyDuplicacy(table t, string key) {
    return t.index.find(key) != t.index.end(); 
}

vector<map<string, string>> insert(table t, vector<map<string, string>> rows) {
    error err;
    // check table existence
    if (currentDB.tables.find(t.name) == currentDB.tables.end()) {
        err.msg = "Table " + t.name + " does not exist";
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
                err.msg = "Attribute '" + attr->first + "' cannot be null";
                throwError(err);
            }
        }
        // primary key uniqueness
        if (checkKeyDuplicacy(t, row[t.primaryKey])) {
            err.msg = "Detected duplicate value " + row[t.primaryKey] +  " on primary key " + t.primaryKey;
            throwError(err);
            continue; 
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
    return rows;
}

void deleteRecord(table t, vector<map<string, string>> rows) {
    error err;
    // check table existence
    if (currentDB.tables.find(t.name) == currentDB.tables.end()) {
        err.msg = "Table " + t.name + " does not exist";
        throwError(err);
    }
    else {
        for (auto itr = rows.begin(); itr != rows.end(); itr++) {
            map<string, string> row = *itr;
            fstream record;
            string file = t.name + "/" + row[t.primaryKey];
            if(ifstream(file)==false) {
                err.msg = "File does not exist";
            } else {
                remove(file);
                map<string, bool>::iterator it;
                it = t.index.find(row[t.primaryKey])
                t.index.erase(it);
            }
        }
    }
}

bool writeTableRecordFile(map<string, table> tables) {
    fstream tablesRecord;
    tablesRecord.open(TABLE_RECORDS, ios::out);

    for (auto& entry : tables) {
        tablesRecord << entry.first << '\n';
    }
    return true;
}

bool dropTable(vector<string> names) {
    for (auto itr = names.begin(); itr != names.end(); itr++) {
        string name = *itr;

        if (currentDB.tables.find(name) == currentDB.tables.end()) {
            error err;
            err.msg = "Table '" + name + "' does not exist";
            throwError(err);
            continue;
        }
        currentDB.tables.erase(name);
    }
    return writeTableRecordFile(currentDB.tables);
}