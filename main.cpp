#include <iostream>
#include "db.cpp"

using namespace std;

int main() {
    chdir(ROOT_URL.c_str());
    database db;
    db.name = "apple";
    createDB(db);
    useDB((char*)db.name.c_str());
    table t;
    t.name = "t";
    attribute attr;
    attr.name = "a";
    t.attributes["a"] = attr;
    t.primaryKey = "a";
    createTable(t);
    vector<map<string, string>> v;
    map<string, string> m;
    m["a"] = "a1";
    v.push_back(m);
    insert(currentDB.tables["t"], v);
}
