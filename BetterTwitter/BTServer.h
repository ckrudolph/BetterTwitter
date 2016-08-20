#include "stdafx.h"

#pragma once
#include <windows.h>      // Needed for all Winsock stuff
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <stdio.h>          // Needed for printf()
#include <stdlib.h>         // Needed for exit()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <fcntl.h>          // Needed for file i/o stuff
#include <process.h>      // Needed for _beginthread() and _endthread()
#include <stddef.h>       // Needed for _threadid
#include <windows.h>      // Needed for all Winsock stuff
#include <sys\stat.h>     // Needed for file i/o constants
#include <io.h>           // Needed for file i/o stuff
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include "BTDBI.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

const int PORT_NUM = 1100;  // Arbitrary port number for the server

class BTServer
{
public:
	BTServer()
	{}

static void disp_last_err(string err_str);

static void startup();

static void waitForConnection(void *);

static void handleConnection(void *);


};