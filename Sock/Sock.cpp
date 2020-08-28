#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string> 
#include <fstream>

#pragma comment(lib,"Wsock32.lib")

#define PORT 44445
#define ADDR "127.0.0.1"
//#define EOF "%%ENDFILE%%" // Данные при окончании файла
BOOL IS_CONNECT = false;

int _connect(SOCKET* sock);                       // Подключение к ожидающему сокету 
int _bind(SOCKET* sock, SOCKET *new_sock);        // Создание ожидающего сокета
int FTP_recive_data(SOCKET* sock);                     // Прием данных от ожидающего сокета
int FTP_send_data(SOCKET* sock);                  // Отправка данных на подключенный сокет


	setlocale(LC_ALL, "Russian");
    /* Инициализация WinSock */
	WSADATA ws_startup;
	if (FAILED(WSAStartup(MAKEWORD(1, 1), &ws_startup)))
	{
		int error = WSAGetLastError();
		printf("Init failed: " + error);
		return 0;
	}
	printf("Init success.\n");

	/* Создание сокета */

	if (INVALID_SOCKET == (*sock = socket(AF_INET, SOCK_DGRAM, 0)))
	{
		int error = WSAGetLastError();
	    printf("Create socket failed: " + error);
	}
	printf("Create socket success.\n");

	/* Отправка данных / Прием данных / Выход */
	printf("Send file - 1\nRecive file - 2\nExit - 3\n>> ");
	char responce;
	scanf("%c", &responce);

	switch (responce)
	{
	case '1':
		FTP_send_data(&sock);
		break;
	case '2':
		FTP_recive_data(&sock);
		break;
	case '3':
		exit(0);
	default:
		printf("Mistake - input data");
		return 0;
	}
	


/* Прием данных от ожидающего сокета*/
int FTP_recive_data(SOCKET* sock)
{   std::cout << "Create file to path: " << std::endl;
	std::string path;
	std::cin >> path;

	int result = _connect(sock);
	if (result != 0)
	{
		printf("ERROR connect : " + result);
		exit(0);
	}

    int file_size = 0;     // Размер скачиваемого файла
	//std::string name_file; // Имя создаваемого файла
    char buf[256];      

    /* Получение данных о имени файла и его расширении */
	recv(*sock, buf, sizeof(buf), 0);
	sscanf(buf, "%d", &file_size);
	std::cout << "file size: " + file_size << std::endl;
	

	/* Создание файла */
	std::ofstream o_file(path, std::ios_base::binary | std::ios_base::app);
	
	

	/* Прием файла по 1024 byte*/
	int read = 0;
	char buff[1024];
	do
	{
		int readeble = recv(*sock, buff, sizeof(buff), 0);
		o_file.write(buff, sizeof(buff));
		read = read + readeble;

	} while (file_size > read);

	o_file.close();
	closesocket(*sock);
    printf("\nRecive file - success.\n");
	system("\npause\n");
	
	return 0;
}

/* Подключение к ожидающему сокету */
int _connect(SOCKET* sock)
{
	// Объявим переменную для хранения адреса
	sockaddr_in addr;
	// тип адреса (TCP/IP)
	addr.sin_family = AF_INET;
	//адрес сервера. Т.к. TCP/IP представляет адреса в числовом виде, то для перевода
	// адреса используем функцию inet_addr.
	addr.sin_addr.S_un.S_addr = inet_addr(ADDR);
	// Порт. Используем функцию htons для перевода номера порта из обычного в //TCP/IP представление.
	addr.sin_port = htons(PORT);

	// Дальше выполняем соединение:
	if (FAILED(SOCKET_ERROR == (connect(*sock, (sockaddr*)&addr, sizeof(addr)))))
	{
		
		int error = WSAGetLastError();
		return error;
	
	}

	IS_CONNECT = true;
	printf("You connected to server.");
	
	return 0;
}

/* Отправка файла по байтово на ожидающий сокет */
int FTP_send_data(SOCKET* sock)
{
	printf("\nPath to file for sender: ");
	std::string path_to_file;
	std::cin >> path_to_file;


	SOCKET new_sock; // Получатель файла

	int result = _bind(sock, &new_sock);
	if (result != 0)
	{
		printf("ERROR bind: " + result);
		system("pause");
	}

	std::ifstream i_file(path_to_file, std::ios_base::binary);
	
	if (!i_file.is_open())
	{
		printf("File not exists");
		system("pause");
		exit(0);
	}

	/* Отправка данных о размере файла получателю */
    struct stat stat_buf;
	stat(path_to_file.c_str(), &stat_buf);
	int file_size = stat_buf.st_size;
	char buff_size_data[256];
	sprintf(buff_size_data, "%d", file_size);
	send(new_sock, buff_size_data, sizeof(buff_size_data), 0);

	/* Данные о имени и расширенни файла */

	/*int pos = path_to_file.find_last_of('\\');
	std::string temp = path_to_file.substr(pos + 1) + "+" + std::to_string(file_size) + "\0";
	std::cout << temp;
	char buff_name_data[256] =  temp.c_str() 
	send(new_sock, buff_name_data, sizeof(buff_name_data), 0);

	Sleep(10); 

	/* Отправка файла получателю потоком по 1024 byte */

    char buffer[1024];

	while (true)
	{
		i_file.read(buffer, sizeof(buffer));
		send(new_sock, buffer, sizeof(buffer), 0);
		if (i_file.eof())
			break;
	}

	i_file.close();
	printf("\nFile sends to client, programm finished...\n");
	system("pause");
	return 0;
	
}

/* Создание ожидающего сокета */
int _bind(SOCKET* sock, SOCKET* new_sock)
{
	// Объявим переменную для хранения адреса
	sockaddr_in addr_in;
	// тип адреса (TCP/IP)
	addr_in.sin_family = AF_INET;
	//адрес сервера. Т.к. TCP/IP представляет адреса в числовом виде, то для перевода
	// адреса используем функцию inet_addr.
	addr_in.sin_addr.S_un.S_addr = inet_addr(ADDR);
	// Порт. Используем функцию htons для перевода номера порта из обычного в //TCP/IP представление.
	addr_in.sin_port = htons(PORT);

	if (FAILED(bind(*sock, (sockaddr*)&addr_in, sizeof(addr_in))))
	{  
		int error = WSAGetLastError();
		return error;
	}
	if (FAILED(listen(*sock, 1)))
	{  
		int error = WSAGetLastError();
		return error;
	}

	sockaddr_in new_addr;
	int len_new_addr = sizeof(new_addr); 
	if (FAILED(*new_sock = accept(*sock, (sockaddr*)&new_addr, &len_new_addr)))
	{
		int error = WSAGetLastError();
		return error;
	}
	printf("Socket connected.");
	
	return 0;
}