#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct attribute {
    string name;
};

struct table {
    string name;
    map<string, attribute> attributes;
    string primaryKey;
};

struct database {
    string name;
    map<string, table> tables;
};

map<string, attribute>::iterator attritr;

const string ROOT_URL = "/home/codzilla/potdb/";
const string DATABASE_RECORDS = "db/";
const string TABLE_RECORDS = "tables/";
const int BLOCK_SIZE = 5;
