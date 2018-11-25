# PotDB

## A minimal on-disk storage database engine in C++

 The database stores information in the form of tables, which further contain rows of information with values corresponding to table attributes. Each table has a primary key associated with it. The present implementation forbids the absence of a primary key. An attribute can further contain a constraint on whether or not it allows `NULL` values. When a database db is in usage i.e., `USE DATABASE db`, the database management system parses some special files to fill up some in-memory structs. These structs contain metadata pertaining to the database and its tables. The table struct further maintains a hashmap of primary key values corresponding to each row(dense-indexing). Each row is stored as a separate file in its table directory, which resides in its parent database’s directory.
