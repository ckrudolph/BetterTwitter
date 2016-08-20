// ConsoleApplication2.cpp : main project file.

#include "stdafx.h"
#include "BTDBI.h"

static const char summ[300] = "Data Source=CSLAB\\SQLEXPRESS;Initial Catalog=BTDB;Integrated Security=True";


SqlConnection^ BTDBI::makeConnection()
{
	System::String^ ConnString = gcnew System::String(summ);
	SqlConnection^ conn = gcnew SqlConnection(ConnString);
	try
	{
		conn->Open();
	}
	catch(SqlException ^mySqlEx)
	{	
		int i;
		for( i=0;i<mySqlEx->Errors->Count;i++)
		{
			//cout << "Source={0};Message={1};" << mySqlEx->Errors->default[i]->Source << mySqlEx->Errors->default[i]->Message << "\n";
		}
	}
	return conn;
}

void BTDBI::rgst(string username, string ip)
{

	deleteUser(username);
	deleteIP(ip);
	String^ cmd = gcnew String("");

	SqlConnection^ conn = makeConnection();
	String^ user = gcnew String(username.c_str());
	String^ address = gcnew String(ip.c_str());

	cmd = "USE BTDB; INSERT INTO Users(Username, IPAddress) VALUES('"+user+"','"+address+"')";

	DataSet^ ds = gcnew DataSet();

	SqlDataAdapter^ da = gcnew SqlDataAdapter(cmd, conn);
	da->Fill(ds);
}

void BTDBI::deleteUser(string username)
{
	String^ qr = gcnew String("");

	SqlConnection^ conn = makeConnection();
	String^ user = gcnew String(username.c_str());

	qr = "USE BTDB; DELETE FROM Users WHERE Username='"+user+"'";

	DataSet^ ds = gcnew DataSet();

	SqlDataAdapter^ da = gcnew SqlDataAdapter(qr, conn);
	da->Fill(ds);
}

void BTDBI::deleteIP(string IP)
{
	String^ qr = gcnew String("");

	SqlConnection^ conn = makeConnection();
	String^ address = gcnew String(IP.c_str());

	qr = "USE BTDB; DELETE FROM Users WHERE IPAddress='"+address+"'";

	DataSet^ ds = gcnew DataSet();

	SqlDataAdapter^ da = gcnew SqlDataAdapter(qr, conn);
	da->Fill(ds);
}

void BTDBI::follow(string strFollower, string strFollowee)
{
	unfollow(strFollower, strFollowee);
	SqlConnection^ conn = makeConnection();
	String^ follower = gcnew String(strFollower.c_str());
	String^ followee = gcnew String(strFollowee.c_str());
	String^ cmd = gcnew String("");

	cmd = "USE BTDB; INSERT INTO FOLLOWERS(Follower, Followee) VALUES('"+follower+"','"+followee+"')";

	DataSet^ ds = gcnew DataSet();

	SqlDataAdapter^ da = gcnew SqlDataAdapter(cmd, conn);
	da->Fill(ds);
}

void BTDBI::unfollow(string strFollower, string strFollowee)
{
	SqlConnection^ conn = makeConnection();

	String^ follower = gcnew String(strFollower.c_str());
	String^ followee = gcnew String(strFollowee.c_str());
	String^ cmd = gcnew String("");

	cmd = "USE BTDB; DELETE FROM FOLLOWERS WHERE Follower='"+follower+"' And Followee='"+followee+"'";

	DataSet^ ds = gcnew DataSet();

	SqlDataAdapter^ da = gcnew SqlDataAdapter(cmd, conn);
	da->Fill(ds);
}

string BTDBI::getFollowers(string username)
{
	SqlConnection^ conn = makeConnection();
	string command = "USE BTDB; SELECT * FROM Followers";
	String^ cmd = gcnew String(command.c_str());
	SqlDataAdapter^ da = gcnew SqlDataAdapter(cmd, conn);

	DataSet^ ds = gcnew DataSet();
	da->Fill(ds, "Followers");
	DataTable^ followerTable = ds->Tables["Followers"];

	command = "USE BTDB; SELECT * FROM Users";
	cmd = gcnew String(command.c_str());
	da = gcnew SqlDataAdapter(cmd, conn);
	DataSet^ ds2 = gcnew DataSet();
	da->Fill(ds2, "Users");
	DataTable^ userTable = ds2->Tables["Users"];
	String^ IPList = gcnew String("");

	for(int i = 0; i < followerTable->Rows->Count; i++)
	{
		bool found = false;
		string follower = msclr::interop::marshal_as<std::string>(followerTable->Rows[i]->ItemArray[0]->ToString());
		string followee = msclr::interop::marshal_as<std::string>(followerTable->Rows[i]->ItemArray[1]->ToString());
		if(followee.compare(username) == 0)
		{
			for(int j = 0; j < userTable->Rows->Count; j++ && !found)
			{
				string user = msclr::interop::marshal_as<std::string>(userTable->Rows[j]->ItemArray[0]->ToString());
				if(follower.compare(user) == 0)
				{
					IPList += userTable->Rows[j]->ItemArray[1]->ToString()+",";
					found = true;
				}	
			}
		}
	}
	if(IPList->Length > 0)
	{
		IPList = IPList->Substring(0, IPList->Length-1);
	}
	else
	{
		IPList = gcnew String("No followers");
	}
	return msclr::interop::marshal_as<std::string>(IPList);
}

string BTDBI::getUsers()
{
	SqlConnection^ conn = makeConnection();
	SqlDataAdapter^ da = gcnew SqlDataAdapter("USE BTDB; select * from Users", conn);
	DataSet^ ds = gcnew DataSet();
	DataTable^ userTable = ds->Tables["Users"];

	String^ s = gcnew String("");
	//Console::WriteLine(playerTable->Rows[0]->ItemArray[0]);
	printf("Rows = <%d>\n", userTable->Rows->Count);
	for(int i = 0; i<userTable->Rows->Count; i++)
	{
		//cout << i;
		printf("UName = <%s>\n", userTable->Rows[i]->ItemArray[0]->ToString());
		printf("IP = <%s>\n", userTable->Rows[i]->ItemArray[1]->ToString());

		s += ""+userTable->Rows[i]->ItemArray[0]+","+userTable->Rows[i]->ItemArray[1];
		printf("s = <%s>\n", s->ToString());
	}
	return  msclr::interop::marshal_as<std::string>(s);
}



/*#ifdef _UNICODE
int wmain(void)
#else
int main(void)
#endif
{
char summ[300] = "Data Source=CSLAB\\SQLEXPRESS;Initial Catalog=BTDB;Integrated Security=True";
//SqlConnection^ cn;
System::String^ ConnString = gcnew System::String(summ);
SqlConnection^ conn = gcnew SqlConnection(ConnString);

try
{
conn->Open();
}
catch(SqlException ^mySqlEx)
{	
int i;
for( i=0;i<mySqlEx->Errors->Count;i++)
{
//cout << "Source={0};Message={1};" << mySqlEx->Errors->default[i]->Source << mySqlEx->Errors->default[i]->Message << "\n";
}

}

SqlDataAdapter^ da = gcnew SqlDataAdapter("select * from Users", conn);
DataSet^ ds = gcnew DataSet();
da->Fill(ds, "Users");

ds->WriteXml("MyDataSet.xml");
DataTable^ playerTable = ds->Tables["Users"];
//Returns Test
Console::WriteLine(playerTable->Rows[0]->ItemArray[0]);
}*/

//int main(array<System::String ^> ^args)
//{
//    Console::WriteLine(L"Hello World");
//    return 0;
//}
