#include <bits/stdc++.h>
#include "help.cpp"
#include "parser.cpp"
using namespace std;

int main() {
	chdir(ROOT_URL.c_str());
	cout<<"---------------------------------------------------Welcome to "<<db_name<<"!"<<"---------------------------------------------------"<<endl;
	cout<<"Type `help` to get information about the usage"<<endl;
	while(true)
	{
		cout<<"->";
		string input;
		getline(cin, input);

		if(input=="help") {
			handle_help();
		}

		else if(input=="quit") {
			exit(0);
		}

		else {
			parse_query(input);
		}
	}
}