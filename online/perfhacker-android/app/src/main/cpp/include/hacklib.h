#ifndef PERFHACKER_HACKLIB_H
#define PERFHACKER_HACKLIB_H

#ifdef WITH_ANDROID
#include <jni.h>
#endif

#include "msm_kgsl_priv.h"

#include <vector>
#include <string>

#define MACRO_GROUP KGSL_PERFCOUNTER_GROUP_LRZ
#define MACRO_COUNTER_NUM_1 (14)
#define MACRO_COUNTER_NUM_2 (15)
#define MACRO_COUNTER_NUM_3 (20)

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
#ifdef WITH_ANDROID
int hacklib_data_keepcollect(std::vector<struct hacklib_datapoint> &data, bool *keep_collect_p, bool realtime, JNIEnv *env);
#else
int hacklib_data_keepcollect(std::vector<struct hacklib_datapoint> &data, bool *keep_collect_p, bool realtime);
#endif

#endif //PERFHACKER_HACKLIB_H