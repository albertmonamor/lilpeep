#pragma once
#include "tcp_socket.h"
#include <sstream>
#include "json.hpp"


using namespace nlohmann;


class http:tcp_socket
{
public:

    http(const char* ip, int port=80, string index="/", string params="");

    string get();

    json post();

    string index;
    string params;
    string method = "GET";
    const char* host;

private:
    
    string s_http_packet(string method);
    
};