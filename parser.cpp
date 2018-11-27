#include <bits/stdc++.h>
#include "db.cpp"

using namespace std;

// every query shall end with a ' ;' i.e. space followed by a semi colon
struct parsed_query {
	//string created_db;
	string used_db;
	string dropped_db;
	//string created_table;
	//string used_table;
	string dropped_table;
	//bool selected_all_columns;
	//bool deleted_all_columns;
	//bool is_where_clause;
	//string inserted_table;
	//string selected_table;
	//string delete_from_table;
	//string where_condition;
	//vector<string> selected_columns;
	//vector<string> deleted_columns;
	//vector<string> inserted_values;
	//string where_clause;
};

vector<string> split(string str, char delimiter) {
  	vector<string> internal;
  	stringstream ss(str);
  	string tok;

  	while(getline(ss, tok, delimiter)) {
    	internal.push_back(tok);
  	}

	return internal;
}

void handleUseDB(string name) {
	if (useDB(name)) {
		cout<<"Switched to database '" + name + "'\n";
	}
}

void handleCreateDB(string name) {
	database db;
	db.name = name;
	if (createDB(db)) {
		cout<<"Success! Database '" + name + "' has been created\n";
	}
}

void handleCreateTable(string name, vector<string> attributeNames, vector<string> nullAllowed, string primaryKey) {
	table t;
	t.name = name;
	map<string, attribute> attributes;

	for (auto& attr : attributeNames) {
		struct attribute a;
		a.name = attr;
		attributes[name] = a;
	}

	for (auto& attr : nullAllowed) {
		attributes[attr].nullAllowed = true;
	}

	t.attributes = attributes;
	createTable(t);
	return;
}

void displayRows(vector<map<string, string>> rows) {
	for (auto& row : rows) {
		cout<<"========"<<endl;
		for (auto& attr : row) {
			cout<<attr.first<<" : "<<attr.second<<endl;
		}
	}
}

void handleSimpleSelect(vector<string> columns, string tableName, vector<string> where) {
	struct selectFromTableOnPrimaryKey s;
	s.columns = columns;
	s.table = tableName;
	s.where = where;
	vector<map<string, string>> rows;
	rows = selectOnPrimaryKeyCondition(s);
	displayRows(rows);
	return;
}

void handleInsert(string tableName, vector<map<string, string>> rows) {
	table t = currentDB.tables[tableName];
	vector<map<string, string>> insertedRows;
	displayRows(rows);
	return;
}


void parse_query(string user_input) {
	parsed_query query;
	error err;
	vector<string> delimited_query = split(user_input, ' ');

	if (delimited_query.size() == 0) {
		return;
	}
	string definer = delimited_query[0];
	delimited_query.erase(delimited_query.begin());
	transform(definer.begin(), definer.end(), definer.begin(), ::tolower);

	if(definer == "use") {
		if (delimited_query.size() == 0) {
			err.msg = "No database specified for USE";
			throwError(err);
			return;
		}
		handleUseDB(delimited_query[1]);
		return;
	}

	else if(definer == "create") {

		if (delimited_query.size() == 0) {
			err.msg = "No object specified for CREATE";
			throwError(err);
			return;
		}
		string object_used = delimited_query[1];
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower);

		if(object_used == "database") {
			handleCreateDB(object_used);
			return;
		}

		if(object_used == "table") {
			delimited_query.erase(delimited_query.begin());

			if (delimited_query.size() == 0) {
				err.msg = "Table properties not specified";
				throwError(err);
				return;
			}
			string tableName = delimited_query[1];
			delimited_query.erase(delimited_query.begin());
			string primaryKey = "";
			vector<string> attributes;
			vector<string> nullAllowed;
	
			for (auto itr = delimited_query.begin(); itr != delimited_query.end(); itr++) {
				bool isPrimaryKey = false;
				bool hasNullConstraint = false;
				string word = *itr;
				// primary key
				if (word[0] == '.') {
					word = word.substr(1, word.size()-1);
					if (word.size() == 0) {
						err.msg = "Invalid attribute specified as primary key";
						throwError(err);
						return;
					}
					if (primaryKey != "") {
						err.msg = "Only one attribute can be the primary key";
						throwError(err);
						return;
					}
					isPrimaryKey = true;
				}
				// null allowed
				if (word[word.length() - 1] == '+') {
					word.pop_back;
					if (word.size() == 0) {
						err.msg = "Attribute name cannot be empty";
						throwError(err);
						return;
					}
					hasNullConstraint = true;
				}

				attributes.push_back(word);
				if (isPrimaryKey) {
					primaryKey = word;
				}
				if (hasNullConstraint) {
					nullAllowed.push_back(word);
				}

				handleCreateTable(tableName, attributes, nullAllowed, primaryKey);
			}
			return;
		}
		err.msg = "Invalid object for CREATE";
		throwError(err);
		return;
	}

	else if(definer=="drop")
	{
		string object_used = delimited_query[1];
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower);
		if(object_used=="database") {
			query.dropped_db=delimited_query[2];
		}
		else if(object_used=="table") {
			query.dropped_table=delimited_query[2];
		}
	}

	else if(definer=="select") {
		if (delimited_query.size() == 0 || delimited_query.size() == 1) {
			err.msg = "Incorrect syntax in select query";
			throwError(err);
			return;
		}
		vector<string> columns;
		columns = split(delimited_query[0], ',');
		string tableName = delimited_query[1];
		vector<string> where;
		delimited_query.erase(delimited_query.begin(), delimited_query.begin() + 1);
		where = delimited_query;
		handleSimpleSelect(columns, tableName, where);
		return;
	}

	else if(definer=="insert") {
		if (delimited_query.size() == 0 || delimited_query.size() == 1 || delimited_query.size() == 2) {
			err.msg = "Incorrect syntax for INSERT query";
			throwError(err);
			return;
		}
		string tableName = delimited_query[1];
		delimited_query.erase(delimited_query.begin(), delimited_query.begin() + 1);
		vector<string> rows = delimited_query;
		vector<map<string, string>> rowsToInsert;		

		for (auto& row : rows) {
			map<string, string> rowToInsert;
			vector<string> keyValuePairs = split(row, ',');

			for (auto& keyValuePair : keyValuePairs) {
				vector<string> keyValue = split(keyValuePair, ':');
				if (keyValue.size() != 2) {
					err.msg = "Invalid syntax in insert query";
					throwError(err);
					return;
				}
				string key = keyValue[0];
				string value = keyValue[1];
				rowToInsert[key] = value;
			}

			rowsToInsert.push_back(rowToInsert);
		}
		handleInsert(tableName, rowsToInsert);
		return;
	}

	else if(definer=="delete") {
		if (delimited_query.size() == 0 || delimited_query.size() == 1) {
			err.msg = "Invalid syntax in DELETE query";

			if (delimited_query[0] == "table") {
				delimited_query.erase(delimited_query.begin());
				dropTable(delimited_query);
			}
		}

		return;
	}
	else {
		err.msg = "Invalid syntax";
		throwError(err);
		return;	
	}
}

int main()
{
	string input;
	getline(cin, input);
	parse_query(input);
	return 0;
}