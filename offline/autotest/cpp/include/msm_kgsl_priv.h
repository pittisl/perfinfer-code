#ifndef PERFHACKER_MSM_KGSL_PRIV_H
#define PERFHACKER_MSM_KGSL_PRIV_H

#define KGSL_IOC_TYPE 0x09

#define KGSL_PERFCOUNTER_GROUP_CP 0x0
#define KGSL_PERFCOUNTER_GROUP_RBBM 0x1
#define KGSL_PERFCOUNTER_GROUP_PC 0x2
#define KGSL_PERFCOUNTER_GROUP_VFD 0x3
#define KGSL_PERFCOUNTER_GROUP_HLSQ 0x4
#define KGSL_PERFCOUNTER_GROUP_VPC 0x5
#define KGSL_PERFCOUNTER_GROUP_TSE 0x6
#define KGSL_PERFCOUNTER_GROUP_RAS 0x7
#define KGSL_PERFCOUNTER_GROUP_UCHE 0x8
#define KGSL_PERFCOUNTER_GROUP_TP 0x9
#define KGSL_PERFCOUNTER_GROUP_SP 0xA
#define KGSL_PERFCOUNTER_GROUP_RB 0xB
#define KGSL_PERFCOUNTER_GROUP_PWR 0xC
#define KGSL_PERFCOUNTER_GROUP_VBIF 0xD
#define KGSL_PERFCOUNTER_GROUP_VBIF_PWR 0xE
#define KGSL_PERFCOUNTER_GROUP_MH 0xF
#define KGSL_PERFCOUNTER_GROUP_PA_SU 0x10
#define KGSL_PERFCOUNTER_GROUP_SQ 0x11
#define KGSL_PERFCOUNTER_GROUP_SX 0x12
#define KGSL_PERFCOUNTER_GROUP_TCF 0x13
#define KGSL_PERFCOUNTER_GROUP_TCM 0x14
#define KGSL_PERFCOUNTER_GROUP_TCR 0x15
#define KGSL_PERFCOUNTER_GROUP_L2 0x16
#define KGSL_PERFCOUNTER_GROUP_VSC 0x17
#define KGSL_PERFCOUNTER_GROUP_CCU 0x18
#define KGSL_PERFCOUNTER_GROUP_LRZ 0x19
#define KGSL_PERFCOUNTER_GROUP_CMP 0x1A
#define KGSL_PERFCOUNTER_GROUP_ALWAYSON 0x1B
#define KGSL_PERFCOUNTER_GROUP_SP_PWR 0x1C
#define KGSL_PERFCOUNTER_GROUP_TP_PWR 0x1D
#define KGSL_PERFCOUNTER_GROUP_RB_PWR 0x1E
#define KGSL_PERFCOUNTER_GROUP_CCU_PWR 0x1F
#define KGSL_PERFCOUNTER_GROUP_UCHE_PWR 0x20
#define KGSL_PERFCOUNTER_GROUP_CP_PWR 0x21
#define KGSL_PERFCOUNTER_GROUP_GPMU_PWR 0x22
#define KGSL_PERFCOUNTER_GROUP_ALWAYSON_PWR 0x23
#define KGSL_PERFCOUNTER_GROUP_MAX 0x24

#define KGSL_PERFCOUNTER_NOT_USED 0xFFFFFFFF
#define KGSL_PERFCOUNTER_BROKEN 0xFFFFFFFE

struct kgsl_perfcounter_get {
    unsigned int groupid;
    unsigned int countable;
    unsigned int offset;
    unsigned int offset_hi;
    unsigned int __pad;
};

#define IOCTL_KGSL_PERFCOUNTER_GET \
        _IOWR(KGSL_IOC_TYPE, 0x38, struct kgsl_perfcounter_get)

struct kgsl_perfcounter_put {
    unsigned int groupid;
    unsigned int countable;
    unsigned int __pad[2];
};

#define IOCTL_KGSL_PERFCOUNTER_PUT \
        _IOWR(KGSL_IOC_TYPE, 0x39, struct kgsl_perfcounter_put)

struct kgsl_perfcounter_read_group {
    unsigned int groupid;
    unsigned int countable;
    unsigned long long value;
};

struct kgsl_perfcounter_read {
    struct kgsl_perfcounter_read_group *reads;   // before *reads, still a __user macro
    unsigned int count;
    unsigned int __pad[2];
};

#define IOCTL_KGSL_PERFCOUNTER_READ \
    _IOWR(KGSL_IOC_TYPE, 0x3B, struct kgsl_perfcounter_read)

#endif //PERFHACKER_MSM_KGSL_PRIV_H