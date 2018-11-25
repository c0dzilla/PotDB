#include <bits/stdc++.h>

using namespace std;

struct query {
	string created_db;
	string used_db;
	string dropped_db;
	string created_table;
	string used_table;
	string dropped_table;
	bool selected_all_columns;
	bool deleted_all_columns;
	string inserted_table;
	string selected_table;
	string delete_from_table;
	vector<string> selected_columns;
	vector<string> deleted_columns;
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
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower)
		if(object_used=="database") {
			used_db=delimited_query[2];
		}
		else if(object_used=="table") {
			used_table=delimited_query[2];
		}
	}

	else if(definer=="create")
	{
		string object_used = delimited_query[1];
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower)
		if(object_used=="database") {
			created_db=delimited_query[2];
		}
		else if(object_used=="table") {
			created_table=delimited_query[2];
		}
	}

	else if(definer=="drop")
	{
		string object_used = delimited_query[1];
		transform(object_used.begin(), object_used.end(), object_used.begin(), ::tolower)
		if(object_used=="database") {
			dropped_db=delimited_query[2];
		}
		else if(object_used=="table") {
			dropped_table=delimited_query[2];
		}
	}

	else if(definer=="select")
	{
		if(delimited_query[1]=="*")
		{
			selected_all_columns = true;
		}
		else
		{
			selected_columns = split(delimited_query[1], ',') // in case specific attributes are selected
		}
		// delimited_query[2] = the from clause therefore skipping
		selected_table = delimited_query[3];
		transform(selected_table.begin(), selected_table.end(), selected_table.begin(), ::tolower);

	}
	else if(definer=="delete")
	{
		if(delimited_query[1]=="*")
		{
			deleted_all_columns = true; // while deleting a row, dont forget to add a *
		}
		else
		{
			deleted_columns = split(delimited_query[1], ',') // in case specific attributes are selected
		}
		delete_from_table = delimited_query[3];
		transform(delete_from_table.begin(), delete_from_table.end(), delete_from_table.begin(), ::tolower);
	}
	else if(definer=="insert")
	{
		handle_insert(delimited_query);
	}
}

void handle_insert(string delimited_query) {
	
}

int main()
{
	string input;
	getline(cin, input);
	parse_query(input);
	return 0;
}