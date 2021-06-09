#!/usr/bin/python3

HEADER = """#include "models.h"

// Usage instruction:
//
// 1. add the following lines into models.h:
//    extern struct perfinfer_model model_{};
// 2. add "&model_{}," into model_p list.

struct perfinfer_model model_{} = {
    .tb = {
"""

TAIL = "};\n"



if __name__ == '__main__':
    # read device info
    with open('./output/deviceinfo.txt') as f:
        txt = f.readlines()
    mymodel = txt[0].strip()
    # read threshold value
    with open('./threshold.txt') as f:
        txt = f.readlines()
    mythreshold = int(txt[0])
    # read table
    with open('./output/analyze.txt') as f:
        txt_table = f.readlines()
    with open('./{}.cpp'.format(mymodel), 'w') as fout:
        fout.write(HEADER.replace('{}', mymodel))
        for i in txt_table:
            tmp_list = i.split(' ')[1:]
            fout.write('            {');
            for j in tmp_list:
                fout.write('{}, '.format(j))
            fout.write('},\n')
        fout.write('    },\n')
        fout.write('    .threshold = {},\n'.format(mythreshold))
        fout.write('    .modelname = "{}"\n'.format(mymodel));
        fout.write(TAIL);
