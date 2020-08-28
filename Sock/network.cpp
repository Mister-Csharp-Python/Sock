#include "network.h"

using namespace std;

Network::Network()
{
	/* ѕроверка библиоткеки на работоспособность */
	WSADATA ws_startup;
	if (FAILED(WSAStartup(MAKEWORD(1, 1), &ws_startup)))
	{
		int error = WSAGetLastError();
		cout << "Init failed" + error << endl;
		return;
	}
	cout << "Init success" << endl;

	/* —оздание серверного сокета, используем UDP протокол */

	if (INVALID_SOCKET == (this->sock = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		int error = WSAGetLastError();
		cout << "Create socket failed" + error << endl;
		return;
	}
	cout << "Create socket success" << endl;
	this->is_init = true;
}

/* Connected in server if mistake: return - false */
bool Network::_connect(const char* address, int port)
{
	this->addr.sin_family = AF_INET;
	this->addr.sin_addr.S_un.S_addr = inet_addr(address);
	this->addr.sin_port = htons(port);

	if (FAILED(SOCKET_ERROR == (connect(this->sock, 
		(sockaddr*)&this->addr, sizeof(this->addr)))))
	{

		int error = WSAGetLastError();
		cout << "Failed connect " + error << endl;
		return false;
	}

	this->is_connect = true;
	cout << "You connected " << endl;

	return 0;
}


/* In mode server */
bool Network::_bind(const char* address, int port)
{
	this->addr.sin_family = AF_INET;
	this->addr.sin_addr.S_un.S_addr = inet_addr(address);
	this->addr.sin_port = htons(port);

	if (FAILED(::bind(this->sock, (sockaddr*)&this->addr, sizeof(this->addr))))
	{
		int error = WSAGetLastError();
		cout << "Failed bind " + error << endl;
		return false;
	}
	cout << "Socket binds. " << endl;
	return true;
}

/* Wait connected, if new connection: return - object new socket */
SOCKET Network::_listen()
{
	SOCKET new_socket;
	if (FAILED(listen(this->sock, 1)))
	{

		int error = WSAGetLastError();
		cout << "Failed bind::listen " + error << endl;
		return false;
	}

	int len_new_addr = sizeof(this->new_addr);
	if (FAILED(new_socket = accept(this->sock, (sockaddr*)&this->new_addr, &len_new_addr)))
	{
		int error = WSAGetLastError();
		cout << "Failed bind::accept " + error << endl;
		return false;
	}
	cout << "Socket connected"<< endl;
	return new_socket;
}

void Network::send_str(const char* message, SOCKET* sock)
{
	send(*sock, message, sizeof(message) + 1, 0);
}

bool Network::recive_file_data(SOCKET* sock)
{
	/* recive data from server */
	char buf[256];           
	memset(buf, sizeof(buf), NULL);
	recv(*sock, buf, 256, 0);
	if (strcmp(buf, "500_Internal_Server_Error"))
	{
		cout << "Failed" << endl;
		MessageBox(NULL, "Internal Server Error", "Failed", 0);
		exit(0);
	}
	printf(buf);
	system("pause");
	char c_str_size[128];
	char c_str_name[128];
	memset(c_str_size, NULL, sizeof(c_str_size));
	memset(c_str_name, NULL, sizeof(c_str_name));


	/* Unboxing data for file  */
	int i;
	for (i = 0; i < 256; i++)
	{
		if (buf[i] != '+')
		{
			c_str_size[i] = buf[i];
			continue;
		}
		i++;
		int j = 0;
		while (buf[i] != NULL)
		{
			c_str_name[j] = buf[i];
			j++;
			i++;
		}
		break;
	}

	printf("name: %s / size: %s\n",c_str_name, c_str_size);
	
	string file_name = string(c_str_name); // name of load file 
	long int file_size = atoi(c_str_size); // size of load file
	
	int one_proc = file_size / 100;
	cout << "name file: " + string(c_str_name) + "\nfile size: " + string(c_str_size) << endl << endl;
	
	/* Create file */
	std::ofstream o_file(dir_downloads + file_name, std::ios_base::binary);

	/* Resice data file of 1024 byte */
	int read = 0;
	char buffer[1024];
	int count = 0;
	do
	{
		int readeble = recv(*sock, buffer, 1024, 0);
		o_file.write(buffer, readeble);
		read = read + readeble;
		count += readeble;
		if (count > (one_proc * 2))
		{
			cout << "\\";
			count = 0;
		}
			

	} while (file_size > read);

	o_file.close();
	MessageBox(NULL, "Submission completed successfully", "Message", 0);
	return true;
}


bool Network::send_file_data(string path_to_file, SOCKET *new_sock)
{

	std::ifstream i_file(path_to_file, std::ios_base::binary);

	if (!i_file.is_open())
	{
		cout << "Failed" << endl;
		MessageBox(NULL, "File not found", "Failed", 0);
		send(*new_sock, "500_Internal_Server_Error", 25, 0);
		exit(0);
	}

	/* Send data of file, size/name */

    char buff_data[256]; // buffer for data
	memset(buff_data, 0, sizeof(buff_data));
	/* file size */
	struct stat stat_buf;
	stat(path_to_file.c_str(), &stat_buf);
	int file_size = stat_buf.st_size;
	if (file_size == 0)
	{
		cout << "Failed" << endl;
		send(*new_sock, "500_Internal_Server_Error", 25, 0);
		MessageBox(NULL, "File size: 0", "Failed", 0);
		exit(0);
	}
	/* file name */
    string name_file = path_to_file.substr(path_to_file.find_last_of("\\") + 1, string::npos);
	/* boxing file name and file size to buffer for send */
	sprintf(buff_data, "%d%c%s", file_size,'+',name_file.c_str());
	/* send file data */
	send(*new_sock, buff_data, sizeof(buff_data), 0);

	char buffer[1024];
	while (true)
	{
		i_file.read(buffer, sizeof(buffer));
	    send(*new_sock, buffer, sizeof(buffer), 0);
		if (i_file.eof())
			break;
	}

	i_file.close();
	cout << ("\nFile sends to client, programm finished.") << endl;
	system("pause");
	return true;
}


