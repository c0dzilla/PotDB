#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "constants.cpp"

using namespace std;

struct selectFromTableOnPrimaryKey {
    vector<string> columns;
    string table;
    vector<string> where;
};

struct select {
    struct selectFromTableOnPrimaryKey s;
};

struct createDatabase {
    struct database db;
};

struct createTable {
    struct table t;
};
