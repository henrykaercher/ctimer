#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <time.h>
#include "gdef.h"

typedef enum{
	ARG_HELP,
	ARG_START,
	ARG_STOP,
	ARG_CHECK_ALL,
	ARG_CHECK_CURRENT,
	ARG_ERROR
}arg_type;

typedef enum{
    SQL_CREATE_TABLE,
    SQL_START_SESSION,
    SQL_STOP_SESSION,
	SQL_GET_ALL,
    SQL_GET_CURRENT,
    SQL_QUERY_COUNT
} sql_query;

static const char *sql[SQL_QUERY_COUNT] = {
	[SQL_CREATE_TABLE] =
		"CREATE TABLE IF NOT EXISTS sessions ("
		"id INTEGER PRIMARY KEY,"
		"task TEXT NOT NULL,"
		"start INTEGER NOT NULL,"
		"end INTEGER"
		");",

	[SQL_START_SESSION] =
		"INSERT INTO sessions(task, start) VALUES(?, ?);",

	[SQL_STOP_SESSION] =
		"UPDATE sessions "
		"SET end = ? "
		"WHERE end IS NULL;",

	[SQL_GET_ALL] =
		"SELECT task, start, end "
		"FROM sessions;",

	[SQL_GET_CURRENT] =
		"SELECT task, start, end "
		"FROM sessions "
		"WHERE end IS NULL "
		"ORDER BY id DESC LIMIT 1;"
};

void db(arg_type arg, char *task_name);
arg_type arg_holder(u8 argc, char **argv);
void print_time(time_t timestamp);
void print_duration(time_t duration);

u32 main(u8 argc, char **argv){
	arg_type args = arg_holder(argc, argv);
	if(args == ARG_ERROR){
		fprintf(stderr, "Argument not supported, use -h or -help to see the help page.\n");
		return 1;
	}

	if(args == ARG_HELP){
		printf(
			"ctimer - Simple CLI time tracker\n"
			"\n"
			"USAGE\n"
			"    ctimer [OPTION] [TASK]\n"
			"\n"
			"OPTIONS\n"
			"    -s, -start <task>    Start a new task.\n"
			"    -p, -stop            Stop the current task.\n"
			"    -c, -check           Show all recorded sessions.\n"
			"    -h, -help            Display this help message.\n"
			"\n"
			"DEFAULT BEHAVIOR\n"
			"    Running \"ctimer\" without arguments shows the current\n"
			"    active task and its elapsed time.\n"
			"\n"
			"NOTES\n"
			"    Only one task can be active at a time.\n"
			"    Starting a new task automatically stops the previous one.\n"
		);
		return 0;
	}

	char *task = (argc > 2) ? argv[2] : "*unnamed*";
	db(args, task);
	return 0;
}

void db(arg_type arg, char *task_name){
	sqlite3 *db;
	char *err;

	if(sqlite3_open("tracker.db", &db) != SQLITE_OK){
		printf("Error on opening db. \n");
		return;
	}

	//setup
	if(sqlite3_exec(db, sql[SQL_CREATE_TABLE], NULL, NULL, &err) != SQLITE_OK){
		fprintf(stderr, "%s\n", err);
		sqlite3_free(err);
	}

	if(arg == ARG_START){
		sqlite3_stmt *stmt;

		sqlite3_prepare_v2(db, sql[SQL_GET_CURRENT], -1, &stmt, NULL);
		u8 has_current = (sqlite3_step(stmt) == SQLITE_ROW);
		sqlite3_finalize(stmt);
		if(has_current){
			sqlite3_prepare_v2(db, sql[SQL_STOP_SESSION], -1, &stmt, NULL);
			sqlite3_bind_int64(stmt, 1, time(NULL));

			if(sqlite3_step(stmt) != SQLITE_DONE){
				fprintf(stderr, "%s\n", sqlite3_errmsg(db));
			}
			sqlite3_finalize(stmt);
			printf("Old task has been closed.\n");
		}

		sqlite3_prepare_v2(db, sql[SQL_START_SESSION], -1, &stmt, NULL);
		sqlite3_bind_text(stmt, 1, task_name, -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(stmt, 2, time(NULL));

		if(sqlite3_step(stmt) != SQLITE_DONE){
			fprintf(stderr, "%s\n", sqlite3_errmsg(db));
		}
		sqlite3_finalize(stmt);
		printf("Task with the name %s has been started.\n", task_name);
	}
	else if(arg == ARG_STOP){
		sqlite3_stmt *stmt;
		sqlite3_prepare_v2(db, sql[SQL_STOP_SESSION], -1, &stmt, NULL);
		sqlite3_bind_int64(stmt, 1, time(NULL));

		if(sqlite3_step(stmt) != SQLITE_DONE){
			fprintf(stderr, "%s\n", sqlite3_errmsg(db));
		}
		sqlite3_finalize(stmt);
		if(sqlite3_changes(db) == 0) printf("No running task.\n");
		else printf("Current task stopped.\n");
	}
	else if(arg == ARG_CHECK_ALL){
		sqlite3_stmt *stmt;

		sqlite3_prepare_v2(db, sql[SQL_GET_ALL], -1, &stmt, NULL);

		while(sqlite3_step(stmt) == SQLITE_ROW){
			const char *task = (const char *)sqlite3_column_text(stmt, 0);
			sqlite3_int64 start = sqlite3_column_int64(stmt, 1);
			sqlite3_int64 end = sqlite3_column_int64(stmt, 2);

			printf("%s ", task);
			print_time(start);
			printf(" -> ");
			print_time(end);
			printf("\n");
		}

		sqlite3_finalize(stmt);
	}
	else if(arg == ARG_CHECK_CURRENT){
		sqlite3_stmt *stmt;

		sqlite3_prepare_v2(db, sql[SQL_GET_CURRENT], -1, &stmt, NULL);

		if(sqlite3_step(stmt) == SQLITE_ROW){
			const char *task = (const char *)sqlite3_column_text(stmt, 0);
			sqlite3_int64 start = sqlite3_column_int64(stmt, 1);

			time_t duration = time(NULL) - start;

			printf("%s ", task);
			printf("\nStart: ");
			print_time(start);
			printf("\nDuration: ");
			print_duration(duration);
			printf("\n");
		}
		else printf("You don't have any task running. Use -h or -help if need help for using the application.\n");

		sqlite3_finalize(stmt);
	}

	sqlite3_close(db);
}

arg_type arg_holder(u8 argc, char **argv){
  if(argc < 2) return ARG_CHECK_CURRENT;

    if(strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0) return ARG_HELP;
	else if(strcmp(argv[1], "-start") == 0 || strcmp(argv[1], "-s") == 0) return ARG_START;
	else if(strcmp(argv[1], "-stop") == 0 || strcmp(argv[1], "-p") == 0) return ARG_STOP;
	else if(strcmp(argv[1], "-check") == 0 || strcmp(argv[1], "-c") == 0) return ARG_CHECK_ALL;
	else return ARG_ERROR;
}

void print_time(time_t timestamp){
    struct tm *tm = localtime(&timestamp);

	if(timestamp == 0) printf("--running--");
	else{
		printf("%02d/%02d/%04d %02d:%02d:%02d",
			   tm->tm_mday,
			   tm->tm_mon + 1,
			   tm->tm_year + 1900,
			   tm->tm_hour,
			   tm->tm_min,
			   tm->tm_sec);
	}
}

void print_duration(time_t duration){
    int h = duration / 3600;
    int m = (duration % 3600) / 60;
    int s = duration % 60;

    printf("%02d:%02d:%02d", h, m, s);
}
