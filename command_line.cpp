#include <bits/stdc++.h>
#include "constants.cpp"
using namespace std;

void handle_help() {
	cout<<"1. To create a database : ";
	cout<<"create database <db_name> ;"<<endl;
	cout<<"2. To use a database : ";
	cout<<"use database <db_name> ;"<<endl;
	cout<<"3. To delete a database : ";
	cout<<"drop database <db_name>;"<<endl;
	cout<<"4. To create a table : ";
	cout<<"create table <table_name> ;"<<endl;
	cout<<"5. To delete a table : ";
	cout<<"drop table <table_name> ;"<<endl;
	cout<<"6. To query rows from table : ";
	cout<<"select * from <table_name> ;"<<" OR "<<"select * from <table_name> where <condition> ;"<<endl;
	cout<<"7. To insert a row : ";
	cout<<"insert into <table_name> values <a,b,c,d> ;"<<endl;
	cout<<"8. To delete entries :";
	cout<<"delete * from <table_name> ;"<<" OR "<<"delete * from <table_name> where <condition> ;"<<endl;
}

int main()
{
	cout<<"---------------------------------------------------Welcome to "<<db_name<<"!"<<"---------------------------------------------------"<<endl;
	cout<<"Type `help` to get information about the usage"<<endl;
	while(true)
	{
		cout<<"->";
		string input;
		getline(cin, input);
		if(input=="help")
		{
			handle_help();
		}
		if(input=="quit")
		{
			exit(0);
		}
		else
		{
			//handle this
		}
	}
}