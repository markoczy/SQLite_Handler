#include "SQLiteHandler.h"

SQLiteHandler::SQLiteHandler()
{
	lastError = "OK";
}

//// @todo TestMe
SQLiteHandler::~SQLiteHandler()
{
	//delete db;
}

////
//// @brief			Open a Database by name
//// @returns		Function successful
////
bool SQLiteHandler::openDB(std::string name)
{
	int rVal = sqlite3_open(name.c_str(),&db);
	
	if (rVal)
	{
		lastError = sqlite3_errmsg(db);
		return false; 
	}
	else{
		dbIsOpen = true;
		return true;
	}
}

////
//// @brief			Close the Database
//// @returns		Function successful
////
bool SQLiteHandler::closeDB()
{
	if (!dbIsOpen)
	{
		lastError = "No Database is open.";
		return false;
	}
	else
	{
		int rVal=sqlite3_close(db);
		if (rVal){
			lastError = sqlite3_errmsg(db);
			return false;
		}
		else{
			lastError = "OK";
			return true;
		}
	}
}


////
//// @brief			Execute an SQL statement
//// @returns		Function successful
////
bool SQLiteHandler::exec(std::string statement)
{
	//-//-// Exit if no db is open . . .
	// 
	if (!dbIsOpen)
	{
		lastError = "No Database is open.";
		return false;
	}
		
	//-//-// Reinit callback values
	//
	callbacks.clear();
	
	//-//-// Execute Statement: Full Callback
	//
	char *zErrMsg = 0;
	int rVal = sqlite3_exec(db, statement.c_str(), sql_cb_full, this, &zErrMsg);
	
	//-//-// Error Handler
	//
	if (rVal)
	{
		lastError = zErrMsg;
		sqlite3_free(zErrMsg);
		return false;
	}
	
	if (callbacks.size() == 0)
	{
		// No Callback
		callbackW = 0;
		callbackH = 0;
	}
	else
	{
		callbackH = callbacks.size();
		callbackW = callbacks.at(0).azColName.size();
	}

	lastError = "OK";
	return true;

}


////
//// @brief			Execute an SQL statement
////
//// This function will generate no callback
//// If a callback is needed use exec()
////
//// @returns		Function successful
////
bool SQLiteHandler::fastExec(std::string statement)
{
	//-//-// Exit if no db is open . . .
	// 
	if (!dbIsOpen)
	{
		lastError = "No Database is open.";
		return false;
	}

	//-//-// Clear callback to avoid confusion
	//
	callbacks.clear();
	
	//-//-// Execute Statement: No Callback
	//
	char *zErrMsg = 0;
	int rVal = sqlite3_exec(db, statement.c_str(), NULL, this, &zErrMsg);
	
	//-//-// Error Handler
	//
	if (rVal)
	{
		lastError = zErrMsg;
		sqlite3_free(zErrMsg);
		return false;
	}

	callbackH = 0;
	callbackW = 0;


	lastError = "OK";
	return true;

}


////
//// @brief			Execute an SQL statement
//// @returns		Function successful
////
bool SQLiteHandler::fastExecSingleSelect(std::string statement, std::string &aOutVal)
{	
	//-//-// Exit if no db is open . . .
	// 
	if (!dbIsOpen)
	{
		lastError = "No Database is open.";
		return false;
	}

	//-//-// Callback will not be written
	//
	callbacks.clear();

	//-//-// Execute Statement: Single callback
	//
	char *zErrMsg = 0;
	int rVal = sqlite3_exec(db, statement.c_str(), sql_cb_single, &aOutVal, &zErrMsg);

	//-//-// Error Handler
	//
	if (rVal)
	{
		lastError = zErrMsg;
		sqlite3_free(zErrMsg);
		return false;
	}
	
	
	callbackH = 0;
	callbackW = 0;

	return true;

}


////
//// @param[in] colSize max Size of column
////
bool SQLiteHandler::printAllCallback(int colSize)
{
	//-//-// return if no callback found
	//
	if (callbacks.size() == 0)
	{
		lastError = "No callback found...";
		return false;
	}

	//-//-// Init output
	//
	std::cout << "\n\n================== STATEMENT CALLBACK _ ==================\n\n";

	//-//-// Get Table width
	//
	int numCol = callbacks.at(0).argc;
	
	//-//-// Create the separator
	//
	char* sepX = new char[(colSize*numCol) + 1];
	int itSepX = 0;
	for (int iCol = 0; iCol < numCol; iCol++)
	{
		for (int sz = 0; sz < colSize; sz++)
		{
			sepX[itSepX++] = '-';
		}
		sepX[itSepX++] = '-';
	}
	sepX[itSepX++] = '-';
	sepX[itSepX] = '\0';


	//-//-// Print Table Header
	//
	std::cout << sepX << std::endl;
	std::cout << "|";
	for (int iCol = 0; iCol < numCol; iCol++)
	{
		std::cout << std::left << std::setw(colSize) << std::setfill(' ') << callbacks.at(0).azColName.at(iCol).c_str();
		std::cout << "|";
	}
	std::cout << std::endl;
	std::cout << sepX << std::endl;
	std::cout << sepX << std::endl;

	//-//-// Print Table Body
	//
	for (unsigned int iCB = 0; iCB < this->callbacks.size(); iCB++)
	{
		std::cout << "|";
		for (int iCol = 0; iCol < numCol; iCol++)
		{
			std::cout << std::left << std::setw(colSize) << std::setfill(' ') << callbacks.at(iCB).agrv.at(iCol).c_str();
			std::cout << "|";
		}
		std::cout << std::endl;

	}
	std::cout << sepX << std::endl;
	
	//-//-// DeInit Output
	//
	std::cout << "\n\n================== _ STATEMENT CALLBACK ==================\n\n";

	lastError = "OK";
	return true;
}

bool SQLiteHandler::printCallbackAt(int it, int colSize)
{
	//-//-// return if no callback found
	//
	if (callbacks.size() == 0)
	{
		lastError = "No callback found.";
		return false;
	}
	else if ((unsigned int)it >= callbacks.size())
	{
		lastError = "Callback iterator too high.";
		return false;
	}

	std::cout << "\n\n================== STATEMENT CALLBACK _ ==================\n\n";

	//-//-// Get Table width
	//
	int numCol = callbacks.at(0).argc;

	//-//-// Create the separator
	//
	char* sepX = new char[(colSize*numCol) + 1];
	int itSepX = 0;
	for (int iCol = 0; iCol < numCol; iCol++)
	{
		for (int sz = 0; sz < colSize; sz++)
		{
			sepX[itSepX++] = '-';
		}
		sepX[itSepX++] = '-';
	}
	sepX[itSepX++] = '-';
	sepX[itSepX] = '\0';

	//-//-// Print Table Header
	//
	std::cout << sepX << std::endl;
	std::cout << "|";
	for (int iCol = 0; iCol < numCol; iCol++)
	{
		std::cout << std::left << std::setw(colSize) << std::setfill(' ') << callbacks.at(0).azColName.at(iCol).c_str();
		std::cout << "|";
	}
	std::cout << std::endl;
	std::cout << sepX << std::endl;
	std::cout << sepX << std::endl;

	//-//-// Print Table Body
	//
	std::cout << "|";
	for (int iCol = 0; iCol < numCol; iCol++)
	{
		std::cout << std::left << std::setw(colSize) << std::setfill(' ') << callbacks.at(it).agrv.at(iCol).c_str();
		std::cout << "|";
	}
	std::cout << std::endl;
	std::cout << sepX << std::endl;

	//-//-// DeInit Output
	//
	std::cout << "\n\n================== _ STATEMENT CALLBACK ==================\n\n";

	lastError = "OK";

	return true;
}


int	SQLiteHandler::getCallbackWidth()
{
	return callbackW;
}

int	SQLiteHandler::getCallbackHeight()
{
	return callbackH;
}

//// @note x and y are 0 terminated
std::string SQLiteHandler::getCallbackAt(int x, int y)
{
	if (callbackH == 0)
	{
		return "";
	}
	else if (y > callbackH)
	{
		return "";
	}
	else if (x > callbackW)
	{
		return "";
	}
	else
	{
		return callbacks.at(y).agrv.at(x);
	}
}


std::string SQLiteHandler::getCallbackHeaderAt(int x)
{
	if (callbackH == 0)
	{
		return "";

	}
	else
	{
		return callbacks.at(0).azColName.at(x);
	}
}


////
//// @brief			Get the DB Pointer
//// @returns		The Database Pointer
////
//// @deprecated	The DB shouldn't be extracted
////
sqlite3 * SQLiteHandler::getDB()
{
	return db;
}

////
//// @brief			Verify if DB is open
//// @returns		Database is open
////
bool SQLiteHandler::getDBisOpen()
{
	return dbIsOpen;
}

std::string SQLiteHandler::getLastError()
{
	return lastError;
}


void SQLiteHandler::_addCallback(int argc, char **argv, char **azColName)
{
	s_sqlcb cb;

	cb.argc = argc;

	for (int i = 0; i < cb.argc; i++)
	{
		if (argv[i] != NULL)
		{
			cb.agrv.push_back(argv[i]);
		}
		else
		{
			cb.agrv.push_back("");
		}

		if (azColName[i] != NULL)
		{
			cb.azColName.push_back(azColName[i]);
		}
		else
		{
			cb.azColName.push_back("");
		}
	}

	callbacks.push_back(cb);

}


static int sql_cb_full(void *param, int argc, char **argv, char **azColName)
{
	SQLiteHandler *h = (SQLiteHandler*)param;

	h->_addCallback(argc, argv, azColName);

	return 0;
}

static int sql_cb_single(void *param, int argc, char **argv, char **azColName)
{
	/*SQL_Handler *h = (SQL_Handler*)param;
	h->_addCallback(argc, argv, azColName);*/

	if (argc == 0)
	{
		return -1;
	}

	std::string *out = (std::string*) param;
	*out = argv[0];

	return 0;
}
