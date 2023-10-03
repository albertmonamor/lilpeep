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


json listDrives(string fppeep){
    // get root directory /:
    vector<char> drive = get_drives_system();
    


}
