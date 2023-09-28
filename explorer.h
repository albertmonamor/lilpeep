
#pragma once
#include <iostream>
#include <vector>
#include "json.hpp"
#include <cstdlib>
#include "protocol.h"

using namespace nlohmann;
using namespace std;



vector<char> get_drives_system();

json listDrives(string fppeep);

vector<string> base_listdir(string path, bool is_file);

json listdir(string path);

size_t get_sizefile(string path);

bool isDirectory(string path);

bool delete_path(string path);

bool rename_path(string path, string npath);

bool isValidFileToEdit(string path);

bool isValidFileToRead(string path);

bool isFile(string path);

bool isTypeFileUnsupported(string path);

pair<size_t, ofstream> openFile(string path);
