#include "explorer.h"

#include <cwchar>




wchar_t* MBTWChar(string str){

    wstring wstr(str.begin(), str.end());

    wchar_t* wide_char = &wstr[0];

    return wide_char;
}

string WCTMbyte(wchar_t* wch){


    size_t size = wcstombs(nullptr, wch, 0);
    if (size == static_cast<size_t>(-1)){
        return "";
    }
    char* mByte = new char[size+1];
    if (wcstombs(mByte, wch, size + 1) == static_cast<size_t>(-1)) {
        delete[] mByte;
        return"";
    }
    mByte[size] = '\0';
    string multi_bytes(mByte);
    //cleanup
    delete[]mByte;
    // SUCCESS
    return multi_bytes;
}



vector<char> get_drives_system(){
    // irrelevant
    // just root
    vector<char> drive;
    drive.push_back('/');
    return drive;

}

/// @brief for unix need just "/", so,  there is one driver
json listDrives(string fppeep) {
	
	json lDrives = json::array();
	vector<char> drives = get_drives_system();
	for (char drive : drives) {
        // for unix just "/"
		json x_drive = { {"name", "root directory"}, {"part", string(1,drive) } };
	
		lDrives.push_back(x_drive);
	}
	json xpwd = { {"name", "pwd"}, {"part", fppeep} };
	lDrives.push_back(xpwd);
	return { {"drives", lDrives} };
}


vector<string> base_listdir(string path, bool is_file){

    vector<string> list_files;
    DIR* directory = opendir(path.c_str());
    if (!directory){
        return list_files;
    }

    struct dirent* dentry;

    while((dentry = readdir(directory)) != nullptr){

        if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0) {
            continue;
        }
        string filname(dentry->d_name);
        list_files.push_back(filname);
    }
    closedir(directory);
    return list_files;
}


json listdir(string path){

    string _path = path;
	size_t total_size{ 0 };
	json lFiles = json::array();

	// default notice about unknown error
	string error = "path not found";
	json default_file = { {"name", error}, {"size", 0}, {"dis",true} };

    bool is_file = isFile(path.c_str());
	vector<string> list_dir = base_listdir(_path, is_file);
	// valid?
	if (is_file) {
		default_file["name"] = path;
		default_file["size"] = get_sizefile(path.c_str());
		total_size += (int)default_file["size"];
		lFiles.push_back(default_file);
	}

    for (string fn : list_dir) {
        int size = get_sizefile((path +"/"+ fn).c_str());
        bool dis = false;
        if (size == -1){
            dis = false;
            size = 0;
        }
        total_size += size;
        json xfile = { {"name", fn}, {"size", size}, {"dis", dis} };
        lFiles.push_back(xfile);
	}
    if (lFiles.empty()) {
		default_file["name"] = "directory empty";
		lFiles.push_back(default_file);
	}
    // success <!>
    json list_files_info = { {"dir", path},{"files",lFiles}, {"tsize", total_size}};
    return list_files_info;

}

size_t get_sizefile(const char* path){

    struct stat fStatus;

    int status = lstat(path, &fStatus);
    if (status != 0){
        return -1;
    }
    return fStatus.st_size;
}


bool isDirectory(const char* path){
    struct stat fStatus;

    int status = lstat(path, &fStatus);
    if (status !=0){
        return false;
    }
    if (!S_ISDIR(fStatus.st_mode)){
        return false;
    }
    return true;
}


bool delete_path(const char* path){

    bool isdir = isDirectory(path);
    bool isfile = isFile(path);
    bool success{false};

    if (isdir){
        success = !rmdir(path);
    }
    else if (isfile){
        success  = !remove(path);
    }
    return success;
}


bool rename_path(string opath, string npath){
    return rename(opath.c_str(), npath.c_str()) == 0;
}


bool isValidFileToEdit(string path){
    if (!isValidFileToRead(path)){
        return false;
    }
    for (string tFile : TYPE_FILE_BINARY) {
    	// bug: not abs
        
    	if (path.find("."+tFile) != string::npos) {
    		return false;
    	}
    }
    return true;
}



bool isValidFileToRead(string path){

    if (isDirectory(path.c_str())){
        return false;
    }
    else if (!isFile(path.c_str())){
        return false;
    }
    return true;
}






bool isFile(const char* path){

    struct stat fStatus;

    int status = lstat(path, &fStatus);
    if (status != 0){
        return  false;
    }
    else if (!S_ISREG(fStatus.st_mode)){
        return false;
    }

    return true;


}


pair<size_t, int> openFile(string path){

    constexpr size_t b1K = 1024;
    pair<size_t, int> pfile(0, -1);
    size_t fsize = get_sizefile(path.c_str());
    int fDiscrip = open(path.c_str(), O_RDONLY);
    if (fDiscrip == -1){
        return pfile;
    }
    // SUCCESS
    return make_pair(fsize, fDiscrip);


}