#include <cstdio>
#include <cstdint>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <sys/time.h>
#include <signal.h>
#include <cstring>
#include <ctime>
#include <cmath>

#include "hacklib.h"

unsigned int GROUP_CODE[GROUP_TOTAL] = {KGSL_PERFCOUNTER_GROUP_LRZ, KGSL_PERFCOUNTER_GROUP_RAS, KGSL_PERFCOUNTER_GROUP_VPC};
unsigned int COUNTER_NUM[GROUP_TOTAL][COUNTER_EACH] = {
        {13, 14, 15, 18},
        {1, 4, 5, 8},
        {9, 10, 12, 12}};
//        {10, 11, 12, 13}}; // pixel 2 only


struct timeval time_get, time_finish;

// Sleep for correct amount of time so that collected data table works
void _do_sleep() {
    struct timespec tsp;
    tsp.tv_sec = 0;
    tsp.tv_nsec = 3600000;
    nanosleep(&tsp, NULL);
    return;
}

inline void _update_choice(int *curr_choice) {
    *curr_choice = ((*curr_choice) + 1) % GROUP_TOTAL;
}

int _hacklib_recorddp(
        std::vector<struct hacklib_datapoint> &data,
        unsigned long long delta[GROUP_TOTAL][COUNTER_EACH],
        struct timeval *data_timestamp,
        bool is_real_event) {
    struct hacklib_datapoint tmp_dp;
    gettimeofday(&(tmp_dp.timestamp), NULL);
    tmp_dp.timestamp = *data_timestamp;
    for (int i = 0; i < GROUP_TOTAL; i++) {
        for (int j = 0; j < COUNTER_EACH; j++) {
            tmp_dp.values[i][j] = delta[i][j];
        }
    }
    tmp_dp.is_real_event = is_real_event;
    data.push_back(tmp_dp);
    if (data.size() > 20) {
        data.erase(data.begin());
    }
    return 0;
}

int _hacklib_analyzedp(struct kgsl_perfcounter_read_group data_read_group[GROUP_TOTAL][COUNTER_EACH], std::vector<struct hacklib_datapoint> &data) {
    static unsigned long long ori_value[GROUP_TOTAL][COUNTER_EACH] = { 0 };
    static bool already_init = false;
    unsigned long long delta[GROUP_TOTAL][COUNTER_EACH] = { 0 };
    unsigned long long new_value[GROUP_TOTAL][COUNTER_EACH];
    struct timeval tmp_timestamp;
    int ret_value = -1;

    if (! (already_init)) {
        already_init = true;
        for (int i = 0; i < GROUP_TOTAL; i++) {
	    for (int j = 0; j < COUNTER_EACH; j++) {
            ori_value[i][j] = 0;
	    }
        }
    }

    bool this_has_value = false;
    for (int i = 0; i < GROUP_TOTAL; i++) {
	for (int j = 0; j < COUNTER_EACH; j++) {
	    new_value[i][j] = data_read_group[i][j].value;
	    delta[i][j] = new_value[i][j] - ori_value[i][j];
	    ori_value[i][j] = new_value[i][j];
	    if (delta[i][j] != 0) {
		this_has_value = true;
	    }
	}
    }

    gettimeofday(&(tmp_timestamp), NULL);
    if (this_has_value) {
        bool is_real_event = false;
        struct timeval time_sub_result;
        printf("DEBUG: %ld.%06ld:", tmp_timestamp.tv_sec, tmp_timestamp.tv_usec);
	for (int i = 0; i < GROUP_TOTAL; i++) {
	    printf(" %llu %llu %llu %llu;", delta[i][0], delta[i][1], delta[i][2], delta[i][3]);
	}
	printf("\n");

        _hacklib_recorddp(data, delta, &tmp_timestamp, is_real_event);
        gettimeofday(&(time_finish), NULL);
    }

    return ret_value;
}



int hacklib_data_output(std::vector<struct hacklib_datapoint> &data, std::string output_path) {
    FILE *fp;
    fp = fopen(output_path.c_str(), "w");
    for (auto &it : data) {
        fprintf(fp, "%ld.%06ld", it.timestamp.tv_sec, it.timestamp.tv_usec);
        for (int i = 0; i < GROUP_TOTAL; i++) {
            for (int j = 0; j < COUNTER_EACH; j++) {
                fprintf(fp, " %llu", it.values[i][j]);
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}

int hacklib_data_keepcollect(std::vector<struct hacklib_datapoint> &data, bool *keep_collect_p, bool realtime) {
    int ret, fd;
    struct kgsl_perfcounter_get data_get[GROUP_TOTAL][COUNTER_EACH];
    struct kgsl_perfcounter_put data_put[GROUP_TOTAL][COUNTER_EACH];
    struct kgsl_perfcounter_read_group data_read_group[GROUP_TOTAL][COUNTER_EACH];
    struct kgsl_perfcounter_read data_read[GROUP_TOTAL];
    int curr_choice = 0;

    fd = open("/dev/kgsl-3d0", O_RDWR);
    if (fd == -1) {
        perror("open kgsl-3d0 failed");
        return -1;
    }

    for (int i = 0; i < GROUP_TOTAL; i++) {
        memset(&(data_read_group[i]), 0, (sizeof(struct kgsl_perfcounter_read_group) * COUNTER_EACH));
        memset(&(data_read[i]), 0, sizeof(struct kgsl_perfcounter_read));
        for (int j = 0; j < COUNTER_EACH; j++) {
            memset(&(data_get[i][j]), 0, sizeof(struct kgsl_perfcounter_get));
            memset(&(data_put[i][j]), 0, sizeof(struct kgsl_perfcounter_put));
            data_get[i][j].groupid = data_put[i][j].groupid = GROUP_CODE[i];
            data_get[i][j].countable = data_put[i][j].countable = COUNTER_NUM[i][j];
            ret = ioctl(fd, IOCTL_KGSL_PERFCOUNTER_GET, &(data_get[i][j]));
            if (ret) { fprintf(stderr, "%s\n", strerror(errno)); }
            data_read_group[i][j].groupid = data_get[i][j].groupid;
            data_read_group[i][j].countable = data_get[i][j].countable;
        }
        data_read[i].reads = data_read_group[i];
        data_read[i].count = COUNTER_EACH;
    }

    while (*keep_collect_p) {
        _do_sleep();
	for (int i = 0; i < GROUP_TOTAL; i++) {
            ret = ioctl(fd, IOCTL_KGSL_PERFCOUNTER_READ, &(data_read[i]));
            if (ret) { fprintf(stderr, "read: %s\n", strerror(errno)); }
	}
        gettimeofday(&(time_get), NULL);
        int ret = _hacklib_analyzedp(data_read_group, data);
        // output time analysis
        if (ret == 0) {
            printf("START: %ld.%06ld:\n", time_get.tv_sec, time_get.tv_usec);
            printf("FINISH: %ld.%06ld:\n", time_finish.tv_sec, time_finish.tv_usec);
        }
    } // while()

    for (int i = 0; i < GROUP_TOTAL; i++) {
        for (int j = 0; j < COUNTER_EACH; j++) {
            ret = ioctl(fd, IOCTL_KGSL_PERFCOUNTER_PUT, &(data_put[i][j]));
            if (ret) { fprintf(stderr, "put: %s\n", strerror(errno)); }
        }
    }

    return 0;
}
