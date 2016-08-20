// ConsoleApplication2.cpp : main project file.

#include "stdafx.h"

#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <msclr/marshal_cppstd.h>

#using <mscorlib.dll>
#using <System.dll>
#using <System.Data.dll>
#using <System.Xml.dll>

using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;

class BTDBI
{
public:
	BTDBI(){};

	BTDBI(char[300]);

	static string getUsers();

	static void rgst(string, string);

	static void deleteUser(string);

	static void follow(string, string);

	static string getFollowers(string);

	static void unfollow(string, string);

	static bool isFollowing(string, string);

	static void deleteIP(string);

private:
	static SqlConnection^ makeConnection();
};

