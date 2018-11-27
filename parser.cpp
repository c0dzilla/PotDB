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

void handleCreateTable(string name, vector<string> attributes, vector<string> notNull, string primaryKey) {
}


void parse_query(string user_input) {
	parsed_query query;
	error err;
	/*
		user query is split on whitespace,
		note that the columns and clauses should be written without spaces
	*/
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

	if(definer == "create") {

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
			vector<string> notNull;
	
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
				// null not allowed
				if (word[word.length() - 1] == '-') {
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
					notNull.push_back(word);
				}

				handleCreateTable(tableName, attributes, notNull, primaryKey);
			}

			handleCreateTable(tableName, delimited_query);
		}

		else {
			err.msg = "Invalid object for CREATE";
			throwError(err);
			return;
		}
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

	else if(definer=="select")
	{
		if(delimited_query[1]=="*")
		{
			query.selected_all_columns = true;
		}
		else
		{
			query.selected_columns = split(delimited_query[1], ','); // in case specific attributes are selected
		}
		// delimited_query[2] = the from clause therefore skipping
		query.selected_table = delimited_query[3];
		transform(query.selected_table.begin(), query.selected_table.end(), query.selected_table.begin(), ::tolower);
		if(delimited_query[4]!=";")
		{
			transform(delimited_query[4].begin(), delimited_query[4].end(), delimited_query[4].begin(), ::tolower);
			if(delimited_query[4]=="where")
			{
				query.is_where_clause = true;
				query.where_condition = delimited_query[5];
			}
		}

	}
	else if(definer=="delete")
	{
		if(delimited_query[1]=="*")
		{
			query.deleted_all_columns = true; // while deleting a row, dont forget to add a *
		}
		else
		{
			query.deleted_columns = split(delimited_query[1], ','); // in case specific attributes are selected
		}
		query.delete_from_table = delimited_query[3];
		transform(query.delete_from_table.begin(), query.delete_from_table.end(), query.delete_from_table.begin(), ::tolower);
		if(delimited_query[4]!=";")
		{
			transform(delimited_query[4].begin(), delimited_query[4].end(), delimited_query[4].begin(), ::tolower);
			if(delimited_query[4]=="where")
			{
				query.is_where_clause = true;
				query.where_condition = delimited_query[5];
			}
		}
	}
	else if(definer=="insert")
	{
		// delimited_query[1] in this case will be the into clause, therefore skipping
		query.inserted_table = delimited_query[2];
		// delimited_query[3] will be the clause "values", therefore skipping
		query.inserted_values = split(delimited_query[4], ','); // syntax will be INSERT INTO <table_name> VALUES a,b,c,d ;
	}
	else
	{
		cout<<"Invalid query"<<endl;	
	}
}

int main()
{
	string input;
	getline(cin, input);
	parse_query(input);
	return 0;
}