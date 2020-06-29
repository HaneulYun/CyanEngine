#pragma once
#include <locale.h>

#define UNICODE
#include <sqlext.h>
#include <Windows.h>
#include <string>
#include "protocol.h"
#include "Client.h"
#include "etc.h"

using namespace std;

class Database
{
public:
	
	/************************************************************************
	/* HandleDiagnosticRecord : display error/warning information
	/*
	/* Parameters:
	/*      hHandle ODBC handle
	/*      hType Type of handle (SQL_HANDLE_STMT, SQL_HANDLE_ENV, SQL_HANDLE_DBC)
	/*      RetCode Return code of failing command
	/************************************************************************/
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
	{
		SQLSMALLINT iRec = 0;
		SQLINTEGER  iError;
		WCHAR       wszMessage[1000];
		WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

		if (RetCode == SQL_INVALID_HANDLE) {
			fwprintf(stderr, L"Invalid handle!\n");
			return;
		}
		while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
			(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS) {
			// Hide data truncated..
			if (wcsncmp(wszState, L"01004", 5)) {
				fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
			}
		}
	}

	void show_error() {
		printf("error\n");
	}

	Inform add_player_in_database(string name)
	{
		SQLHENV henv;
		SQLHDBC hdbc;
		SQLHSTMT hstmt = 0;
		SQLRETURN retcode;
		SQLWCHAR szUser_name[MAX_ID_LEN];
		SQLINTEGER dUser_xpos, dUser_ypos, dUser_level, dUser_exp, dUser_hp;
		SQLLEN cbName = 0, cbXpos = 0, cbYpos = 0, cbLevel = 0, cbExp = 0, cbHp = 0;

		wstring wname;
		wname.assign(name.begin(), name.end());
		Inform p;
		p.x = -1;

		setlocale(LC_ALL, "korean");

		// Allocate environment handle  
		retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

		// Set the ODBC version environment attribute  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

			// Allocate connection handle  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

				// Set login timeout to 5 seconds  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

					// Connect to data source  
					retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182031", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

					// Allocate statement handle  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						//printf("ODBC connect OK!\n");
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

						wstring tmp = L"EXEC add_newplayer " + wname;
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp.c_str(), SQL_NTS);
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							//printf("Select OK\n");
							// Bind columns 1, 2, and 3  
							retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szUser_name, MAX_ID_LEN, &cbName);
							retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &dUser_xpos, 100, &cbXpos);
							retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &dUser_ypos, 100, &cbYpos);
							retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &dUser_level, 100, &cbLevel);
							retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &dUser_exp, 100, &cbExp);
							retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &dUser_hp, 100, &cbHp);

							// Fetch and print each row of data. On an error, display a message and exit.  
							for (int i = 0; ; i++) {
								retcode = SQLFetch(hstmt);
								if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
									show_error();
								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
								{
									//replace wprintf with printf
									//%S with %ls
									//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
									//but variadic argument 2 has type 'SQLWCHAR *'
									//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
				
									dUser_xpos = rand() % WORLD_WIDTH;
									if (dUser_xpos >= 17 && dUser_xpos <= 23)
										dUser_ypos = 5;
									else
										dUser_ypos = rand() % 5;

									strcpy_s(p.m_name, name.c_str());
									p.x = dUser_xpos;
									p.y = dUser_ypos;
									p.level = dUser_level;
									p.exp = dUser_exp;
									p.hp = dUser_hp;
								}
								else
								{
									break;
								}
							}
						}

						// Process data  
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							SQLCancel(hstmt);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						}

						SQLDisconnect(hdbc);
					}
					else HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
					SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
				}
			}
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
		}
		return p;
	}

	Inform find_name_in_database(string name)
	{
		SQLHENV henv;
		SQLHDBC hdbc;
		SQLHSTMT hstmt = 0;
		SQLRETURN retcode;
		SQLWCHAR szUser_name[MAX_ID_LEN];
		SQLINTEGER dUser_xpos, dUser_ypos, dUser_level, dUser_exp, dUser_hp;
		SQLLEN cbName = 0, cbXpos = 0, cbYpos = 0, cbLevel = 0, cbExp = 0, cbHp = 0;

		wstring wname;
		wname.assign(name.begin(), name.end());
		Inform p;
		p.x = -1;

		setlocale(LC_ALL, "korean");

		// Allocate environment handle  
		retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

		// Set the ODBC version environment attribute  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

			// Allocate connection handle  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

				// Set login timeout to 5 seconds  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

					// Connect to data source  
					retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182031", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

					// Allocate statement handle  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						//printf("ODBC connect OK!\n");
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

						wstring tmp = L"EXEC select_samename " + wname;
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp.c_str(), SQL_NTS);
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							//printf("Select OK\n");
							// Bind columns 1, 2, and 3  
							retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szUser_name, MAX_ID_LEN, &cbName);
							retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &dUser_xpos, 100, &cbXpos);
							retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &dUser_ypos, 100, &cbYpos);
							retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &dUser_level, 100, &cbLevel);
							retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &dUser_exp, 100, &cbExp);
							retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &dUser_hp, 100, &cbHp);

							// Fetch and print each row of data. On an error, display a message and exit.  
							for (int i = 0; ; i++) {
								retcode = SQLFetch(hstmt);
								if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
									show_error();
								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
								{
									//replace wprintf with printf
									//%S with %ls
									//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
									//but variadic argument 2 has type 'SQLWCHAR *'
									//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
									//wprintf_s(L"find name %d: %s %d %d %d %d %d\n", i + 1, szUser_name, dUser_xpos, dUser_ypos, dUser_level, dUser_exp, dUser_hp);
									strcpy_s(p.m_name, name.c_str());
									p.x = dUser_xpos;
									p.y = dUser_ypos;
									p.level = dUser_level;
									p.exp = dUser_exp;
									p.hp = dUser_hp;
								}
								else
								{
									break;
								}
							}
						}

						// Process data  
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							SQLCancel(hstmt);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						}

						SQLDisconnect(hdbc);
					}
					else HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
					SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
				}
			}
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
		}
		return p;
	}

	void set_data_in_database(string name, int x, int y, int level, int exp, int hp)
	{
		SQLHENV henv;
		SQLHDBC hdbc;
		SQLHSTMT hstmt = 0;
		SQLRETURN retcode;
		SQLWCHAR szUser_name[MAX_ID_LEN];
		SQLINTEGER dUser_xpos, dUser_ypos, dUser_level, dUser_exp, dUser_hp;
		SQLLEN cbName = 0, cbXpos = 0, cbYpos = 0, cbLevel = 0, cbExp = 0, cbHp = 0;

		wstring wname;
		wname.assign(name.begin(), name.end());

		setlocale(LC_ALL, "korean");

		// Allocate environment handle  
		retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

		// Set the ODBC version environment attribute  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

			// Allocate connection handle  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

				// Set login timeout to 5 seconds  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

					// Connect to data source  
					retcode = SQLConnect(hdbc, (SQLWCHAR*)L"game_db_odbc_2017182031", SQL_NTS, (SQLWCHAR*)NULL, 0, NULL, 0);

					// Allocate statement handle  
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
						//printf("ODBC connect OK!\n");
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

						wstring tmp = L"EXEC set_userdata " + wname + L", " + to_wstring(x) + L", " + to_wstring(y) + L", " + to_wstring(level) + L", " + to_wstring(exp) + L", " + to_wstring(hp);
						retcode = SQLExecDirect(hstmt, (SQLWCHAR*)tmp.c_str(), SQL_NTS);
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							//printf("Select OK\n");
							// Bind columns 1, 2, and 3  
							retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, szUser_name, MAX_ID_LEN, &cbName);
							retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &dUser_xpos, 100, &cbXpos);
							retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &dUser_ypos, 100, &cbYpos);
							retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &dUser_level, 100, &cbLevel);
							retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &dUser_exp, 100, &cbExp);
							retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &dUser_hp, 100, &cbHp);

							// Fetch and print each row of data. On an error, display a message and exit.  
							for (int i = 0; ; i++) {
								retcode = SQLFetch(hstmt);
								if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
									show_error();
								if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
								{
									//replace wprintf with printf
									//%S with %ls
									//warning C4477: 'wprintf' : format string '%S' requires an argument of type 'char *'
									//but variadic argument 2 has type 'SQLWCHAR *'
									//wprintf(L"%d: %S %S %S\n", i + 1, sCustID, szName, szPhone);  
									//wprintf_s(L"set data %d: %s %d %d %d %d %d\n", i + 1, szUser_name, dUser_xpos, dUser_ypos, dUser_level, dUser_exp, dUser_hp);
								}
								else
									break;
							}
						}

						// Process data  
						if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
							SQLCancel(hstmt);
							SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
						}

						SQLDisconnect(hdbc);
					}
					else HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
					SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
				}
			}
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
		}
	}
};