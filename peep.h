#pragma once
#include "analyze.h"
#include "protocol.h"
#include "peep_socket.h"
#include <thread>



class peep: peep_socket
{

public:
    peep();

    void run();

    void proto_communication();

    void mode_ping();

    void mode_shell(json info);

    void mode_shell_terminal(json info);

    void mode_screen(json info);

    void mode_explorer(json info);

    void mode_streamer(json info);

    void mode_signal(json info);

    void cleanup();


};

