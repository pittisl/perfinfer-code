#include <cstdio>
#include <cstring>
#include <csignal>

#include "hacklib.h"


static bool keep_collect = true;
extern char names[81][64];

void _stop_sighandler(int sig) {
    keep_collect = false;
    return;
}

int main(int argc, char *argv[]) {
    std::vector<struct hacklib_datapoint> data;
    struct sigaction act;

    // set up signal handler, when sigint/sigterm received,
    // stop collection and dump data to output file
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    act.sa_handler = _stop_sighandler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    // preallocate at least 60 seconds worth of memory
    data.reserve(400000);         // around 16 MiB

    // enter infinite loop of collecting data
    hacklib_data_keepcollect(data, &keep_collect, true);

    // output result data, only when realtime=false
    //hacklib_data_output(data, std::string("./output.txt"));

    return 0;
}
