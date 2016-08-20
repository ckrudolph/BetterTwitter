#include "stdafx.h"
#include "BTClient.h"





void BTClient::disp_last_err(string err_str)
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

void BTClient::startup()
{
  WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
  WSADATA wsaData;                              // Stuff for WSA functions
  WSAStartup(wVersionRequested, &wsaData);
  HANDLE threadH1 = (HANDLE)_beginthread(UDPListen, 4096, (void *) NULL);
  if((int)threadH1 < 0)
      disp_last_err("** ERROR: _beginthread1 failed: ");
  HANDLE threadH2 = (HANDLE)_beginthread(clientThread, 4096, (void *) NULL);
  if((int)threadH2 < 0)
      disp_last_err("** ERROR: _beginthread2 failed: ");
  HANDLE allThreads[] = {threadH1, threadH2};
  WaitForMultipleObjects((DWORD)2, allThreads, true, INFINITE);
}

void BTClient::UDPListen(void* in_arg)
{
	int                  server_s;        // Server socket descriptor
	struct sockaddr_in   server_addr;     // Server Internet address
	struct sockaddr_in   client_addr;     // Client Internet address
	struct in_addr       client_ip_addr;  // Client IP address
	int                  addr_len;        // Internet address length
	char                 in_buf[4096];    // Input buffer for data
	int                  retcode;         // Return code

	server_s = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_s < 0)
		disp_last_err("** ERROR: socket() failed: ");


	server_addr.sin_family = AF_INET;                 // Address family to use
	server_addr.sin_port = htons(CLIENT_PORT_NUM);           // Port number to use
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
	retcode = bind(server_s, (struct sockaddr *)&server_addr,
		sizeof(server_addr));

	if (retcode < 0)
		disp_last_err("** ERROR: bind() failed: ");

	while (true) {
		addr_len = sizeof(client_addr);
		retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0,
			(struct sockaddr *)&client_addr, &addr_len);
		if (retcode < 0)
			disp_last_err("** ERROR: recvfrom() failed: ");

		// Copy the four-byte client IP address into an IP address structure
		memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

		in_buf[retcode] = '\0';

		// Print an informational message of IP address and port of the client
		cout << "IP address of client = " << inet_ntoa(client_ip_addr)
			<< ", port = " << ntohs(client_addr.sin_port) << endl;

		// Output the received message
		cout << "Received from client: " << in_buf << endl;
	}
}

void BTClient::clientThread(void *in_arg)
{
	string cmd = "";
	while (cmd.compare("quit") != 0) {
		cout << "Menu Options:\n";
		cout << "UDP: send a message over UDP to a particular address\n";
		cout << "TCP: send a message to the TCP server\n";
		cout << "quit: exit the BetterTwitter client\n";
		cin >> cmd;
		if (cmd.compare("UDP") == 0) {
			int                  client_s;        // Client socket descriptor
			struct sockaddr_in   server_addr;     // Client Internet address
			string               IP_addr;
			char                 out_buf[4096];   // Output buffer for data
			int                  retcode;         // Return code

			cout << "Enter the IP address of the client you want to send to\n";
			cin >> IP_addr;
			cout << "Enter the message that you want to send\n";

			string message;
			cin.ignore();
			getline(cin, message);

			
			client_s = socket(AF_INET, SOCK_DGRAM, 0);
			if (client_s < 0)
				disp_last_err("** ERROR: socket() failed: ");


			server_addr.sin_family = AF_INET;                 // Address family to use
			server_addr.sin_port = htons(CLIENT_PORT_NUM);           // Port num to use
			server_addr.sin_addr.s_addr = inet_addr(IP_addr.c_str()); // IP address to use

			const char* msg = message.c_str();
			strcpy_s(out_buf, msg);

			// Now send the message to server.  The "+ 1" is for the end-of-string
			// delimiter
			retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
				(struct sockaddr *)&server_addr, sizeof(server_addr));
			if (retcode < 0)
				disp_last_err("** ERROR: sendto() failed: ");
			
			retcode = closesocket(client_s);
			if (retcode < 0)
				disp_last_err("** ERROR: closesocket() failed: ");
		}
		else if (cmd.compare("TCP") == 0) {
		  int                  client_s;        // Client socket descriptor
		  struct sockaddr_in   server_addr;     // Server Internet address
		  char                 out_buf[4096];   // Output buffer for data
		  char                 in_buf[4096];    // Input buffer for data
		  int                  retcode;         // Return code
		  string			   IP_ADDR;
		  
		  cout << "Enter the IP address of the TCP server\n";
		  cin >> IP_ADDR;

		  client_s = socket(AF_INET, SOCK_STREAM, 0);
		  if (client_s < 0)
			disp_last_err("** ERROR: socket() failed: ");


		  // Fill-in the server's address information and do a connect with the
		  // listening server using the client socket - the connect() will block.
		  server_addr.sin_family = AF_INET;                 // Address family to use
		  server_addr.sin_port = htons(CLIENT_PORT_NUM);           // Port num to use
		  server_addr.sin_addr.s_addr = inet_addr(IP_ADDR.c_str()); // IP address to use
		  retcode = connect(client_s, (struct sockaddr *)&server_addr,
			sizeof(server_addr));
		  if (retcode < 0)
			disp_last_err("** ERROR: connect() failed: ");


		  // Send to the server using the client socket
		  cout << "Enter the command that you want to send to the server\n";
		  
			string message;
			cin.ignore();
			getline(cin, message);
			const char* msg = message.c_str();
			strcpy_s(out_buf, msg);

		  retcode = send(client_s, out_buf, (strlen(out_buf) + 1), 0);
		  if (retcode < 0)
			disp_last_err("** ERROR: send() failed: ");


		  // Receive from the server using the client socket
		  retcode = recv(client_s, in_buf, sizeof(in_buf), 0);
		  if (retcode < 0)
			disp_last_err("** ERROR: recv() failed: ");

		  // Output the received message
		  cout << "Received from server: " << in_buf << endl;


		  retcode = closesocket(client_s);
		  if (retcode < 0)
			disp_last_err("** ERROR: closesocket() failed: ");
		}
	}
	WSACleanup();
	_endthread();
}