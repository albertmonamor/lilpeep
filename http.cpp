#include "http.h"
#include <iostream>



http::http(const char* _ip, int port, string index, string params): tcp_socket(NULL, port),index(index),params(params)
{
	// init convert dns to internet protocol
	this->host = _ip;
	this->ip = GetInterntProtclHost(_ip);

}

string http::get(){
    constexpr size_t b2K = 2048;

    string err = "network error";
    if (ip == nullptr){return "unknown";}
    init_socket();
    if (!is_open){
        return err;
    }

    char buffer[b2K];

    if (open_socket() != 0){
        return err;
    }

    string http_packet = s_http_packet(method);
    memset(buffer, 0, b2K);

    send(lpeep, http_packet.c_str(), http_packet.length(), 0);
    // response
    if (recv(lpeep, buffer, b2K, 0) == -1){ts_cleanup();return err;}

    string response(buffer);
	// init vector string
	std::istringstream iss(response);
	std::vector<std::string> lines;
	std::string line;

    while (std::getline(iss, line, '\n')) {
    lines.push_back(line);
	}
	ts_cleanup();
	return lines[lines.size()-1];
}


json http::post(){

    json error = R"({"error": true})"_json;
	if (ip == nullptr) {
		return error;
	}

    init_socket();
	if (!is_open) {
		return error;
	}
	// buffer auto alloc & dealloc
	char buffer[2048];
	// connect
	if (open_socket() != 0) {
		return error;
	}

    string http_packet = s_http_packet("POST");

    if (send(lpeep, http_packet.c_str(), http_packet.length(), 0) == -1) {
    ts_cleanup();
    return error;
	}

	// RESPONSE
	memset(buffer,0, 2048);
	if (recv(lpeep, buffer, 2048, 0) == -1) {
		ts_cleanup();
		return error;
	}

    	string response(buffer);
	// init vector string
	std::istringstream iss(response);
	std::vector<std::string> lines;

	std::string line;
	while (std::getline(iss, line, '\n')) {
		lines.push_back(line);
	}

    ts_cleanup();
	if (lines.size() == 0) {
		return error;
	}
	json data;
	try {
		data = json::parse(lines[lines.size() - 1]);
	}
	catch (json::exception& err) {
		return data;
	}
	return data;
}


string http::s_http_packet(string method) {
	string http_packet;
	http_packet = method + " " + index + " HTTP/1.1\r\n";
	http_packet += "Host: " + string(host) + "\r\n";
	http_packet += "Content-Type: application/x-www-form-urlencoded\r\n";
	http_packet += "Connection: close\r\n";

	if (method == "POST") {
		http_packet += "Content-Length: " + to_string(params.length()) + "\r\n\r\n";
		http_packet += params;
	}
	else {
		http_packet += "\r\n";
	}
	return http_packet;
}

