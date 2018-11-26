#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct attribute {
    string name;
    bool nullAllowed = false;
};

struct table {
    string name;
    map<string, attribute> attributes;
    string primaryKey = "";
    map<string, bool> index;
};

struct database {
    string name;
    map<string, table> tables;
};

database currentDB;

const string ROOT_URL = "/home/codzilla/potdb/";
const string DATABASE_RECORDS = "db";
const string TABLE_RECORDS = "tables";
const string SCHEMA = "schema";
const string INDEX = "index";
const int BLOCK_SIZE = 5;
