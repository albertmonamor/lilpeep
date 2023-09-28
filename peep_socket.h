#pragma once
#include "tcp_socket.h"
#include "json.hpp"

using namespace nlohmann;
using namespace std;



class peep_socket: tcp_socket
{
public:
    peep_socket();

    // set sock
    int create_set_sock();

    int connect(int error = 110);

    void send_protocol();

    bool set_setting();

    pair<json, char*> get_packet_p();

    void send_packet_p(char*, size_t);

    void peep_cleanup();

    json setting_sock = R"({"timeout": 10})"_json;


};



