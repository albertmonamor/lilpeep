#include "peep.h"
#include "modes.h"



peep::peep(){

}


void peep::run(){

    int so_no = 110;
    while (1){

        so_no = this->connect(so_no);
		cout << "status:" << so_no << endl;
        if (!so_no){
            try{
                this->send_protocol();

                this->proto_communication();
            }
            catch(runtime_error&error){
                cout << "[CRITICAL]:" <<  error.what() << endl;
                continue;
            }

        }
    }
}


void peep::proto_communication() {
	// loop 'til error, (communicate forever)

	while (true) {
		auto peepxPCK = get_packet_p();
		// start mapping and process auto
		if (peepxPCK == BLOCK_ERROR) {
			cout << "peepxPCK:" << peepxPCK << endl;
			this->cleanup();
			break;
		}
		// MODE: ping
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[2]) {
			this->mode_ping();
		}
		// MODE: shell
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[3]) {
			this->mode_shell(peepxPCK.first);
		}
		// MODE: screen
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[4]) {
			this->mode_screen(peepxPCK.first);
		}
		// MODE: explorer
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[5]) {
			this->mode_explorer(peepxPCK.first);
		}
		// MODE: stream up/down
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[6]) {
			this->mode_streamer(peepxPCK.first);
		}
		// MODE: shell terminal
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[7]) {
			this->mode_shell_terminal(peepxPCK.first);
		}
		// MODE: signals
		else if (peepxPCK.first["mode"] == MODE_PACKAGE[9]) {
			this->mode_signal(peepxPCK.first);
		}
		// deallocate 
		delete[] peepxPCK.second;


	}
}


void peep::mode_ping() {

	string mode = MODE_PACKAGE[2];
	json info = {
		{"mode", mode}
	};
	// init pointer for size
	size_t len_packet = 0;
	// create packet following protocol "SERx6"
	char* packet = SetBlockPacket(info, nullptr, 0, len_packet);
	this->send_packet_p(packet, len_packet);
	// cleanup
	delete[] packet;
}

void peep::mode_shell(json info){

	if (info["action"] == "OPEN"){
		SETTING["rs:connect"] = 1;

		std::thread thrd([&] { bMode_shell(setting_sock, info["port_stream"]); });
		this_thread::sleep_for(.3s);

		thrd.detach();
	}
	else{
		SETTING["rs:connect"] = 0; 
	}

	size_t lpacket{0};
	json _info = json::parse("{}");
	char* packet = SetBlockPacket(_info, nullptr, 0, lpacket);
	this->send_packet_p(packet, lpacket);
	// cleanup
	delete[] packet;
}

void peep::mode_shell_terminal(json info){

	if (info["action"] == "OPEN"){
		SETTING["trs:connect"] = 1;
		const char* _app = "NULL"; //string app = info["app"];
		thread thrd([&] {bMode_shell_terminal(setting_sock, info["port_stream"], _app);});
		this_thread::sleep_for(.3s);

		thrd.detach();	
	}
	else if (info["action"] == "CLOSE"){
		SETTING["trs:connect"] = 0;
	}

		// #~: responsing to server, the content is not relevant
	size_t lpacket = 0;
	json _info = json::parse("{}");
	char* packet = SetBlockPacket(_info, nullptr, 0, lpacket);
	this->send_packet_p(packet, lpacket);
	// cleanup
	delete[] packet;
}

void peep::mode_screen(json info){
	if (info["action"] == "OPEN"){

		if (info["kind"] == "TCP"){
			SETTING["scrn:connect"] = 1;

			// init
			thread thrd([&] {bMode_screen(setting_sock,info["port_stream"], SETTING);} );
			this_thread::sleep_for(.3s);
			// disconnect
			thrd.detach();
		}
		else if (info["kind"] == "TCPv2"){
			SETTING["scrn:connect"] = 1;
			thread screenv2([&] {bMode_screen_v2(setting_sock, info["port_stream"], SETTING); });
			this_thread::sleep_for(.3s);
			screenv2.detach();
		}
	}
	else if (info["action"] == "CLOSE") {
		SETTING["scrn:connect"] = 0;
	}	

	// #~: responsing to server, the content is not relevant
	size_t lpacket = 0;
	json _info = json::parse("{}");
	char* packet = SetBlockPacket(_info, nullptr, 0, lpacket);
	this->send_packet_p(packet, lpacket);
	// cleanup
	delete[] packet;
}

void peep::mode_explorer(json info){
	
	if (info["action"] == "OPEN"){
		SETTING["expl:connect"] = 1;

		thread thrd([&]{bMode_explorer(setting_sock, info["port_stream"], SETTING);});
		this_thread::sleep_for(.3s);
		// disconnect
		thrd.detach();
	}
	else{
		SETTING["expl:connect"] = 0;
	}

	// #~: responsing to server, the content is not relevant
	size_t lpacket = 0;
	char* packet = SetBlockPacket(json::parse("{}"), nullptr, 0, lpacket);
	this->send_packet_p(packet, lpacket);
	// cleanup
	delete[] packet;
}

void peep::mode_streamer(json info){

}

void peep::mode_signal(json info){

}


void peep::cleanup(){
	this->peep_cleanup();
	// revalue
	for (auto& value : SETTING) {
		// zero
		value.second = 0;
	};
}