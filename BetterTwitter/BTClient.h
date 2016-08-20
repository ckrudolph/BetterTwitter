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
#include <string>

using namespace std;

const int CLIENT_PORT_NUM = 1100;


class BTClient
{

public:
	BTClient();

	static void disp_last_err(string err_str);

	static void startup();

	static void UDPListen(void *);

	static void clientThread(void *);
};

