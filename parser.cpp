#include <bits/stdc++.h>

using namespace std;

// every query shall end with a ' ;' i.e. space followed by a semi colon
struct parsed_query {
	string created_db;
	string used_db;
	string dropped_db;
	string created_table;
	string used_table;
	string dropped_table;
	bool selected_all_columns;
	bool deleted_all_columns;
	bool is_where_clause;
	string inserted_table;
	string selected_table;
	string delete_from_table;
	string where_condition;
	vector<string> selected_columns;
	vector<string> deleted_columns;
	vector<string> inserted_values;
	string where_clause;
};

vector<string> split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;
 
  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
 
  return internal;
}

void parse_query(string user_input) {

	parsed_query query;

	vector<string> delimited_query = split(user_input, ' '); // user query is split on whitespace, note that the columns and clauses should be written without spaces
	int n = delimited_query.size();
	/*cout<<n<<endl;
	for(int i=0; i<n; i++)
	{
		cout<<delimited_query[i]<<endl;
	}*/
	string definer = delimited_query[0];
	transform(definer.begin(), definer.end(), definer.begin(), ::tolower);

	if(definer=="use")
	{
		string object_used = delimited_query[1];
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower);
		if(object_used=="database") {
			query.used_db=delimited_query[2];
		}
		else if(object_used=="table") {
			query.used_table=delimited_query[2];
		}
	}

	else if(definer=="create")
	{
		string object_used = delimited_query[1];
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower);
		if(object_used=="database") {
			query.created_db=delimited_query[2];
		}
		else if(object_used=="table") {
			query.created_table=delimited_query[2];
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