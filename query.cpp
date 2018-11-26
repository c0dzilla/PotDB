#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "constants.cpp"

using namespace std;

struct select {
    struct selectFromTableOnPrimaryKey s;
};

struct selectFromTableOnPrimaryKey {
    vector<string> columns;
    string table;
    vector<string> where;
};

struct createDatabase {
    struct database db;
};

struct createTable {
    struct table t;
};
