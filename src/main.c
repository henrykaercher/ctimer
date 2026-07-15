#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include "gdef.h"

typedef enum{
	ARG_HELP,
	ARG_START,
	ARG_STOP
}arg_type;

void db(arg_type arg);
arg_type arg_holder(u8 argc, char **argv);

int main(u8 argc, char **argv){
	arg_type args = arg_holder(argc, argv);

	printf("%d\n", args);
	db(args);
	return 0;
}

void db(arg_type arg){
	sqlite3 *db;
	char *err;

	if(sqlite3_open("tracker.db", &db) != SQLITE_OK){
		printf("Error on opening db. \n");
	}

	const char *sql =
		"CREATE TABLE IF NOT EXISTS sessions ("
		"id INTEGER PRIMARY KEY,"
		"task TEXT NOT NULL,"
		"start INTEGER NOT NULL,"
		"end INTEGER"
		");";

	if(sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK){
		fprintf(stderr, "%s\n", err);
		sqlite3_free(err);
	}

	sqlite3_close(db);
}

arg_type arg_holder(u8 argc, char **argv){
	arg_type arg = ARG_HELP;
	if(argc <= 1){
		printf("Not enough arguments. \n");
		return arg;
	}

	if(strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0){
		arg = ARG_HELP;
	}
	else if(strcmp(argv[1], "-start") == 0 || strcmp(argv[1], "-s") == 0){
		arg = ARG_START;
	}
	else if(strcmp(argv[1], "-stop") == 0 || strcmp(argv[1], "-p") == 0){
		arg = ARG_STOP;
	}

	return arg;
}
