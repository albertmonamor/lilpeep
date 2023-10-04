#include "tcp_socket.h"
#include "protocol.h"
#include <cerrno>



list<pair<size_t, char*>> receive_proto(int sock, size_t&lpacket, int buf_size){
    
    char* buffer = new char[buf_size];
    list<pair<size_t, char*>> binary;

    int length_packet{0};
    lpacket = 0;
    // memset
    memset(buffer, 0, buf_size);
    // byte length recv
    int blrecv = recv(sock, buffer, buf_size,0);
    if (0 < blrecv < MAX_LEN_PROTO){
        try{
            length_packet = stoi(buffer);
        }
        catch(exception&error){
            delete[] buffer;
            buffer = nullptr;
            return PACKET_ERROR;
        }
    }
    else{
        delete[] buffer;
        return PACKET_ERROR;
    }

    size_t tlength {0};
    // send
    if (send(sock, OK, strlen(OK), 0) == -1){
        delete[] buffer;
        return PACKET_ERROR;
    }

    while (tlength < length_packet){
        memset(buffer, 0, buf_size);
        int seek = recv(sock, buffer, buf_size, 0);
        if (seek <= 0 || seek == -1){delete[] buffer;return PACKET_ERROR; }

        tlength += seek;
        char* tmp = new char[seek];
        memcpy(tmp, buffer, seek);
        binary.emplace_back(seek, tmp);
    }

    // cleanup
    delete[] buffer;
    lpacket = tlength;
    return binary;

}

int send_proto(int sock, char* packet, size_t lpacket){
    char buffer[64];

    string sendl = to_string(lpacket);
    send(sock, sendl.c_str(), sendl.length(), 0);
    // ok
    recv(sock, buffer, 64, 0);
    // success
    return send(sock, packet, lpacket, 0);
}



tcp_socket::tcp_socket(const char* address, int port, int proto):ip(address), port(port),is_open(0),lpeep(0),proto(proto){  
}


// [f] clean
int tcp_socket::init_socket(){

    this->lpeep = socket(AF_INET, SOCK_STREAM, proto);
	is_open = 1;
	return 0;
}

// [f] clean
int tcp_socket::open_socket(const char* __address, int __port){

    sockaddr_in hint;
    int error = 0;
    hint.sin_family = AF_INET;

    if (__port && __address){
        hint.sin_port = htons(__port);
        inet_pton(AF_INET, __address, &hint.sin_addr);
    }
    else{
        hint.sin_port = htons(port);
        inet_pton(AF_INET, this->ip, &hint.sin_addr);
    }

    if (connect(lpeep, (struct sockaddr*)&hint, sizeof(hint)) == -1){
        this->ts_cleanup();
        return (int)errno;
    }

    is_open = 1;
    return 0;
};


list<pair<size_t, char*>> tcp_socket::get_packet(size_t&lpacket, int buf_size){

    try{
        return receive_proto(this->lpeep, lpacket, buf_size);
    }
    catch(exception&error){
        return PACKET_ERROR;
    }
}


int tcp_socket::send_packet(char* packet, size_t lpacket){
    try{
        return send_proto(lpeep, packet, lpacket);
    }
    catch(exception&error){
        return PACKET_ERROR_I;
    }
}

list<pair<size_t, char*>> tcp_socket::communication(char* packet, size_t lpacket) {

	size_t len_packet;
	// SEND > GET
	send_packet(packet, lpacket);
	return get_packet(len_packet);

}

void tcp_socket::ts_cleanup() {
	close(lpeep);
	lpeep = 0;
	is_open = 0;

}


// -------------- tcp_streamer class --------------

tcp_streamer::tcp_streamer(const char* address, int port, int proto):tcp_socket(address, port, proto) {

}

bool tcp_streamer::ReadFromFile(int&cFile, int sock, size_t&lfile){
    
    char* buffer = new char[b4K];

    ssize_t buffer_read{0};
    size_t bRead{0};

    while (1){
        buffer_read = read(cFile, buffer,b4K);
        if (buffer_read <= 0){
            delete[] buffer;
            return false;
        }
        // send
        send(sock, buffer, buffer_read, 0);
        lfile += buffer_read;

    }
    // cleanup
    delete[] buffer;
    return true;
}




const char* GetInterntProtclHost(const char* hostname){

    struct addrinfo hints, *res, *p;
    int status{0};
    char* ip_address = (char*)malloc(INET6_ADDRSTRLEN);
    memset(ip_address, 0, INET6_ADDRSTRLEN);

    // memset
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname,NULL, &hints, &res)) != 0){
        free(ip_address);
        return nullptr;
    }

    for (p = res; p!=nullptr;p = p->ai_next){
        void* addr;
        
        if (p->ai_family == AF_INET){
            struct sockaddr_in* IPv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(IPv4->sin_addr);
        }
        else{
            continue;
        }

        inet_ntop(p->ai_family, addr, ip_address, INET6_ADDRSTRLEN);
        break;

    }

    // cleanup
    freeaddrinfo(res);
    return ip_address;
}
