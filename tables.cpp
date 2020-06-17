#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <dirent.h>
#include "query.cpp"
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
            err.msg = "Table '" + tableName + "' already exists";
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

map<string, bool> fetchIndex(string path) {
    map<string, bool> index;
    DIR *dir = opendir(path.c_str());
    struct dirent *ent = readdir(dir);
    map<string, bool> invalids;
    invalids[SCHEMA] = true;
    invalids["."] = true;
    invalids[".."] = true;

    while (ent != NULL) {
        if (invalids[ent->d_name]) {
            ent = readdir(dir);
            continue;
        }
        index[ent -> d_name] = true;
        ent = readdir(dir);
    }
    closedir(dir);
    return index;
}

void createTable(table t) {
    error err;
    // primary key enforced(atleast for now)
    if (t.primaryKey == "") {
        err.msg = "No primary key specified";
        throwError(err);
        return;
    }
    if (t.attributes.find(t.primaryKey) == t.attributes.end()) {
        err.msg = "Specified primary key '" + t.primaryKey + "' is not a table attribute";
        throwError(err);
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
    t.index = fetchIndex(path);
    currentDB.tables[t.name] = t;
}

map<string, attribute> prepareAttributes(table& t, string path) {
    string file = path + SCHEMA;
    fstream tableFile;
    map<string, attribute> attributes;
    tableFile.open(file, ios::in | ios::out | ios::app);
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
        t.index = fetchIndex(path);
        tables[tableName] = t;
    }
    tablesRecord.close();
    return tables;
}

error checkKeyDuplicacy(table t, string key) {
    error err;
    err.msg = "";
    if (t.index.find(key) != t.index.end()) {
        err.msg = "Detected duplicate value '" + key + "' on primary key " + t.primaryKey;
    }
    return err;
}

vector<map<string, string>> insert(table t, vector<map<string, string>> rows) {
    error err;
    err.msg = "";
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
                err.msg = "Attribute '" + attr->first + "' does not exist";
                throwError(err);
                rows.erase(itr);
                itr--;
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
                rows.erase(itr);
                itr--;
                valid = false;
                break;
            }
        }
        if (!valid) {
            continue;
        }
        // primary key uniqueness
        err = checkKeyDuplicacy(t, row[t.primaryKey]);
        if (err.msg != "") {
            throwError(err);
            rows.erase(itr);
            itr--;
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

        t.index[row[t.primaryKey]] = true;
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

vector<map<string, string>> selectOnPrimaryKeyCondition(selectFromTableOnPrimaryKey s) {
    error err;
    vector<map<string, string>> rows;
    if (currentDB.tables.find(s.table) == currentDB.tables.end()) {
        err.msg = "Table '" + s.table + "' does not exist";
        throwError(err);
        return rows;
    }
    table t = currentDB.tables[s.table];
    // check for invalid columns
    for (auto& column : s.columns) {
        if (t.attributes.find(column) == t.attributes.end()) {
            err.msg = "Invalid attribute '" + column + "' in SELECT statement";
            throwError(err);
            return rows;
        }
    }
    // if only key is needed, populate from index
    if (s.columns.size()  == 1 && s.columns[0] == t.primaryKey) {
        for (auto& key: s.where) {
            if (t.index.find(key) != t.index.end()) {
                map<string, string> row;
                row[t.primaryKey] = key;
                rows.push_back(row);
            }
        }
        return rows;
    }
    // fetch rows
    for (auto& key : s.where) {
        if (t.index.find(key) != t.index.end()) {
            map<string, string> row;
            fstream record;
            string path = t.name + "/" + key;
            record.open(path, ios::out | ios::in | ios::app);
            for (auto& attr : t.attributes) {
                string line;
                getline(record, line);
                // column should be selected
                if (find(s.columns.begin(), s.columns.end(), attr.first) != s.columns.end()) {
                    row[attr.first] = line;
                }
            }
            rows.push_back(row);
        }
    }

    return rows;
}