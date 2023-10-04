
#pragma once
#include <iostream>
#include <vector>
#include "json.hpp"
#include <cstdlib>
#include "protocol.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace nlohmann;
using namespace std;



string WCTMbyte(wchar_t* wch);

wchar_t* MBTWChar(string wch);


vector<char> get_drives_system();

json listDrives(string fppeep);

vector<string> base_listdir(string path, bool is_file);

json listdir(string path);

size_t get_sizefile(const char* path);

bool isDirectory(const char* path);

bool delete_path(const char* path);

bool rename_path(string path, string npath);

bool isValidFileToEdit(string path);

bool isValidFileToRead(string path);

bool isFile(const char* path);

//bool isTypeFileUnsupported(string path);

pair<size_t, int> openFile(string path);