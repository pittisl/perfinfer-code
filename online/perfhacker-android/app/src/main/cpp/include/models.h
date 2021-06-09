#ifndef EXAMPLE_ATTACKING_APP_MODELS_H
#define EXAMPLE_ATTACKING_APP_MODELS_H

#define CHAR_TOTAL_NUM (81)

struct perfinfer_model {
    unsigned long long tb[CHAR_TOTAL_NUM][12];
    int threshold;
    char *modelname;
};

extern char *names[];
extern struct perfinfer_model *model_p[];

#endif //EXAMPLE_ATTACKING_APP_MODELS_H