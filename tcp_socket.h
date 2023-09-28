#pragma once
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <list>
#include <fstream>
#define SOCKET_ERROR (-1)


using namespace std;



list<pair<size_t, char*>> receive_proto(int, size_t&, int);

int send_proto(int, char*, size_t);

class tcp_socket{

public:
    
    tcp_socket(const char* address, int port, int proto = 0);

    int init_socket();

    int open_socket(const char* __address=0, int __port=0);

    list<pair<size_t, char*>> get_packet(size_t&lpacket, int buf_size=1024);

    int send_packet(char* packet, size_t lpacket);

    list<pair<size_t, char*>> communication(char* packet, size_t lpacket);

    void ts_cleanup();

    const char* ip;
	int			port;
	bool        is_open = 0;
	int         lpeep;

private:
	int BUF_SIZE = 4096;
	int proto = 0;
        
};



class tcp_streamer: tcp_socket{

public:

    tcp_streamer(const char* address, int port, int proto = 0);

    bool ReadFromFile(fstream&hFile, int sock, size_t&lfile);

private:
	const size_t b4K = 4096;

};


const char* GetInterntProtclHost(const char*);