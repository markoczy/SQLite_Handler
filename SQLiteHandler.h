#pragma once

#include <iostream>
#include <iomanip>
#include <vector>

#include "sqlite3.h"


/**
*	@version	0.3.3
*	@author		A.Markoczy
*
*	@change		+getLastError()
*/
#define SQLIH_VERSION "0.3.3"

//-//-// Define
//
#define STRVEC std::vector<std::string>

////
//// @brief Used to handle the SQL Callback
////
struct s_sqlcb
{
	int argc;
	STRVEC agrv;
	STRVEC azColName;
};

////
//// @brief Used to define a 
////
class SQLiteHandler
{
public:
	
	//-//-// Con / Des
	//
	SQLiteHandler();
	~SQLiteHandler();
	
	//-//-// De- / Init
	//
	bool openDB(std::string name);
	bool closeDB();
	
	//-//-// Exec
	//
	bool exec(std::string statement);
	bool fastExec(std::string statement);
	bool fastExecSingleSelect(std::string statement,std::string &aOutVal);

	//-//-// Callback Prints
	//
	bool printCallbackAt(int it, int colSize = 20);
	bool printAllCallback(int colSize = 20);
	
	//-//-// Getters
	//
	std::string getCallbackAt(int x, int y);
	std::string getCallbackHeaderAt(int x);
	bool		getDBisOpen();
	sqlite3 *		getDB(); // Not Using for DLL
	int			getCallbackWidth();
	int			getCallbackHeight();
	std::string getLastError();

	//-//-// System functions: don't use unless you know how!
	//
	void _addCallback(int argc, char **argv, char **azColName);
	void _clearCallback();

private:
	sqlite3 *db = NULL;	
	bool dbIsOpen = false;
	
	std::string lastError;
	
	// Callback pointer array
	std::vector<s_sqlcb> callbacks;
	
	int callbackW = 0;
	int callbackH = 0;


};

//-//-// Callback
//
static int sql_cb_full(void *param, int argc, char **argv, char **azColName);
static int sql_cb_single(void *param, int argc, char **argv, char **azColName);