#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct attribute {
    string name;
};

struct table {
    string name;
    vector<attribute> attributes;
    attribute primaryKey;
};

struct database {
    string name;
    vector<table> tables;
};

const string ROOT_URL = "/home/codzilla/potdb";
const int BLOCK_SIZE = 5;
