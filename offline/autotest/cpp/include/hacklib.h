#ifndef PERFHACKER_HACKLIB_H
#define PERFHACKER_HACKLIB_H


#include "msm_kgsl_priv.h"

#include <vector>
#include <string>

#define GROUP_TOTAL (3)
#define COUNTER_EACH (4)

extern unsigned int GROUP_CODE[GROUP_TOTAL];
extern unsigned int COUNTER_NUM[GROUP_TOTAL][COUNTER_EACH];

struct hacklib_datapoint {
    struct timeval timestamp;      // output of gettimeofday()
    unsigned long long values[GROUP_TOTAL][COUNTER_EACH];   // real values
    bool is_real_event;
};

int hacklib_data_output(std::vector<struct hacklib_datapoint> &data, std::string output_path);
int hacklib_data_keepcollect(std::vector<struct hacklib_datapoint> &data, bool *keep_collect_p, bool realtime);

#endif //PERFHACKER_HACKLIB_H
