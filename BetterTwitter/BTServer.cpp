#include "stdafx.h"
#include "BTServer.h"

using namespace std;



struct conn_info
{
	int connect_s;
	struct sockaddr_in   client_addr;     // Client Internet address
	struct in_addr       client_ip_addr;  // Client IP address
};

void BTServer::disp_last_err(string err_str)
{
	char *s = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	cout << err_str.c_str() << s << endl;
	LocalFree(s);
	exit(-1);
}

void BTServer::startup()
{
	WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
	WSADATA wsaData;                              // Stuff for WSA functions
	WSAStartup(wVersionRequested, &wsaData);
	HANDLE threadH = (HANDLE)_beginthread(waitForConnection, 4096, (void *) NULL);
	if((int)threadH < 0)
		disp_last_err("** ERROR: _beginthread() failed: ");
	WaitForSingleObject(threadH, INFINITE);
}

void BTServer::waitForConnection(void *in_arg)
{
	int                  welcome_s;       // Welcome socket descriptor
	struct sockaddr_in   server_addr;     // Server Internet address
	int                  addr_len;        // Internet address length
	int                  retcode;         // Return code

	struct conn_info connInfo;

	welcome_s = socket(AF_INET, SOCK_STREAM, 0);
	if (welcome_s < 0)
		disp_last_err("** ERROR: socket() failed: ");

	server_addr.sin_family = AF_INET;                 // Address family to use
	server_addr.sin_port = htons(PORT_NUM);           // Port number to use
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
	retcode = bind(welcome_s, (struct sockaddr *)&server_addr,
		sizeof(server_addr));
	if (retcode < 0)
		disp_last_err("** ERROR: bind() failed: ");

	listen(welcome_s, 100);

	while(1)
	{
		cout << "Waiting for accept() to complete..." << endl;
		addr_len = sizeof(connInfo.client_addr);

		connInfo.connect_s = accept(welcome_s, (struct sockaddr *)&connInfo.client_addr, &addr_len);
		if (connInfo.connect_s < 0)
			disp_last_err("** ERROR: accept() failed: ");

		if(_beginthread(handleConnection, 4096, (void *)&connInfo) < 0)
			disp_last_err("** ERROR: _beginthread() failed: ");
	}
}

void BTServer::handleConnection(void *in_arg){
	int                  connect_s;       // Connection socket descriptor
	struct sockaddr_in   client_addr;     // Client Internet address
	struct in_addr       client_ip_addr;  // Client IP address
	struct conn_info	*i;
	char                 in_buf[4096];    // Input buffer for data
	char				out_buf[4096];
	int                  retcode;         // Return code
	string				username;

	i = (conn_info*) in_arg;
	struct conn_info info = *i;
	connect_s = info.connect_s;
	client_addr = info.client_addr;

	memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

	// Print an informational message that accept completed
	cout << "Accept completed (IP address of client = " << inet_ntoa(client_ip_addr) 
		<< ", port = " << ntohs(client_addr.sin_port) << endl;

	/* while(true){
	retcode = recv(connect_s, in_buf, sizeof(in_buf), 0);
	if(retcode < 0)
	{
	cout << "Error in recv()";
	closesocket(connect_s);
	_endthread();
	}
	else
	{
	in_buf[retcode] = '\0';
	cout << "Message from the client: " << in_buf << endl;
	}
	}*/

	while(true)//until client chooses to quit
	{
		// Send to the client using the connect socket
		retcode = recv(connect_s, in_buf, sizeof(in_buf), 0);
		if(retcode <= 0)
		{
			cout << "A user left." << endl;
			in_buf[retcode] = '\0';
			closesocket(connect_s);
			_endthread();
		}
		else
		{
			in_buf[retcode] = '\0';

			//Handle command
			/*
			Register: RGST_userName

			Get followers: GTFL_userName

			Follow: FOLL_flwName

			Remove followees: UFOL_flwName

			----------------------------------------------------------------------------------------------------------

			Generic error: FAIL

			Register successful: PASS
			save the username locally

			Get follower success: SDFL_IP_IP_IP
			assuming that everyone has exactly these 3 followers

			Follow success: PASS
			user exists

			Unfollow success: Pass
			user in target's follower list
			*/

			string input = string(in_buf);
			string cmd = input.substr(0, 4);
			cout << "Received from user: <" << input << ">" << endl;
			if(input.size()>4){
				input = input.substr(5);
			}

			if(cmd.compare("RGST") == 0)
			{
				username = input;
				BTDBI::rgst(username, inet_ntoa(client_ip_addr));
				strcpy(out_buf, "PASS");
				retcode = send(connect_s, out_buf, (strlen(out_buf) + 1), 0);
				if (retcode < 0)
					disp_last_err("** ERROR: register response failed: ");
			}
			else if(cmd.compare("GTFL") == 0)
			{
				strcpy(out_buf, BTDBI::getFollowers(username).c_str());
				retcode = send(connect_s, out_buf, (strlen(out_buf) + 1), 0);
				if (retcode < 0)
					disp_last_err("** ERROR: get followers response failed: ");
			}
			else if(cmd.compare("FOLL") == 0)
			{
				string followee = input;

				BTDBI::follow(username, followee);

				strcpy(out_buf, "PASS");
				retcode = send(connect_s, out_buf, (strlen(out_buf) + 1), 0);
				if (retcode < 0)
					disp_last_err("** ERROR: register response failed: ");
			}
			else if(cmd.compare("UFOL") == 0)
			{
				string followee = input;

				BTDBI::unfollow(username, followee);

				strcpy(out_buf, "PASS");
				retcode = send(connect_s, out_buf, (strlen(out_buf) + 1), 0);
				if (retcode < 0)
					disp_last_err("** ERROR: register response failed: ");
			}
			else if(cmd.compare("GTUS") == 0)
			{
				string table = BTDBI::getUsers();
				strcpy(out_buf, table.c_str());
				retcode = send(connect_s, out_buf, (strlen(out_buf) + 1), 0);
				if (retcode < 0)
					disp_last_err("** ERROR: get user table response failed: ");
			}
		}
	}
}

/*
insert into Users values(
username = "Test"
IPAddress = "192.168.0.1"
)

"insert into Users values(username = \"Test\" IPAddress = \"192.168.0.1\")"

*/