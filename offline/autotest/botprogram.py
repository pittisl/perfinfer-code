#!/data/data/com.termux/files/usr/bin/python3

import time
import sys
import os
import signal
import subprocess
import copy
import atexit
import random
from pathlib import Path
import threading
import sys

# DO NOT RUN INSIDE ZSH OR TMUX.
#
# REMEMBER TO GRANT STORAGE PERMISSION TO TERMUX

###### FIXME! collecting app
RUNNING_BINARY_FILE = "./app"
##### FIXME! interval file
TYPING_INTERVAL_FILE = './pwdproc_output.txt'
OUTPUT_DIR_PREFIX = 'output'
TOTAL_SAMPLES = 350
REPEAT_SAMPLES = 7

APP_LOC = {
        'textfield': (309, 621),
        }

KBD_PROFILE = 'virtual_keyboard_location.txt'

#############################################


TARGET_LOC = APP_LOC
DATABASE_FILE = KBD_PROFILE

##############################################


subp = None     # subprocess

STRING_CHOICE = [
        ('main_1', 'main'),
        ('main_2', 'main'),
        ('main_3', 'main'),
        ('main_4', 'main'),
        ('main_5', 'main'),
        ('main_6', 'main'),
        ('main_7', 'main'),
        ('main_8', 'main'),
        ('main_9', 'main'),
        ('main_0', 'main'),
        ('a', 'main'),
        ('b', 'main'),
        ('c', 'main'),
        ('d', 'main'),
        ('e', 'main'),
        ('f', 'main'),
        ('g', 'main'),
        ('h', 'main'),
        ('i', 'main'),
        ('j', 'main'),
        ('k', 'main'),
        ('l', 'main'),
        ('m', 'main'),
        ('n', 'main'),
        ('o', 'main'),
        ('p', 'main'),
        ('q', 'main'),
        ('r', 'main'),
        ('s', 'main'),
        ('t', 'main'),
        ('u', 'main'),
        ('v', 'main'),
        ('w', 'main'),
        ('x', 'main'),
        ('y', 'main'),
        ('z', 'main'),
        ('main_dot', 'main'),
        ('main_comma', 'main'),
        ('A', 'upper'),
        ('B', 'upper'),
        ('C', 'upper'),
        ('D', 'upper'),
        ('E', 'upper'),
        ('F', 'upper'),
        ('G', 'upper'),
        ('H', 'upper'),
        ('I', 'upper'),
        ('J', 'upper'),
        ('K', 'upper'),
        ('L', 'upper'),
        ('M', 'upper'),
        ('N', 'upper'),
        ('O', 'upper'),
        ('P', 'upper'),
        ('Q', 'upper'),
        ('R', 'upper'),
        ('S', 'upper'),
        ('T', 'upper'),
        ('U', 'upper'),
        ('V', 'upper'),
        ('W', 'upper'),
        ('X', 'upper'),
        ('Y', 'upper'),
        ('Z', 'upper'),
        ('symbol_at', 'symbol'),
        ('symbol_sharp', 'symbol'),
        ('symbol_dollar', 'symbol'),
        ('symbol_underscore', 'symbol'),
        ('symbol_and', 'symbol'),
        ('symbol_minus', 'symbol'),
        ('symbol_plus', 'symbol'),
        ('symbol_leftbracket', 'symbol'),
        ('symbol_rightbracket', 'symbol'),
        ('symbol_slash', 'symbol'),
        ('symbol_asterisk', 'symbol'),
        ('symbol_doublequote', 'symbol'),
        ('symbol_singlequote', 'symbol'),
        ('symbol_colon', 'symbol'),
        ('symbol_semicolon', 'symbol'),
        ('symbol_exclamation', 'symbol'),
        ('symbol_question', 'symbol'),
        ]



###################################################

TOTAL_COLLECT_TIME=45

###################################################

SEARCH_DICT = dict()               # SEARCH_DICT['space'] = (123, 456,)
TYPING_HISTORY = list()
TYPING_INTERVAL_DB = list()        # TYPING_INTERVAL_DB[123] = (0.23, 0.026)
TYPING_INTERVAL_DB_LEN = None

CURR_KBD_LAYOUT = 'main'

# #######################################################

def check_is_root():
    if os.getuid() != 0:
        print('ERROR: you must run it as root!')
        sys.exit(-1)

def do_log(log_str: str):
    print('N: {}'.format(log_str))

def work_init() -> dict:
    # Init virtual keyboard database.
    # Also init TYPING_INTERVAL_DB
    global SEARCH_DICT
    global TYPING_INTERVAL_DB
    global TYPING_INTERVAL_DB_LEN
    with open(DATABASE_FILE) as f:
        all_text = f.readlines()
    all_data = dict()
    for i in all_text:
        line = i.replace('\n', '').split('\t')
        all_data[line[0]] = (line[1], line[2], line[3],)
    SEARCH_DICT = copy.deepcopy(all_data)
    with open(TYPING_INTERVAL_FILE) as f:
        all_text = f.readlines()
    interval_lines = list()
    for i in all_text:
        interval_lines.append(i.split(' '))
    interval_lines_size = len(interval_lines[0]) - 1
    for i in range(interval_lines_size):
        TYPING_INTERVAL_DB.append((float(interval_lines[0][i]), float(interval_lines[1][i])))
    TYPING_INTERVAL_DB_LEN = len(TYPING_INTERVAL_DB)
    return all_data


def find_xy(keyword_str: str) -> tuple:
    try:
        ret = SEARCH_DICT[keyword_str]
    except KeyError:
        raise
    return ret


def get_curr_timestamp() -> float:
    # date +%s.%6N
    # 1620850666.752517
    return time.time()

def exec_input(exec_str: str):
    # Exec a string for touchscreen input.
    os.system('/system/bin/input {}'.format(exec_str))


def exec_am(exec_str: str):
    os.system('/system/bin/am {}'.format(exec_str))

def do_switch_keyboard_layout_decl(layout1, layout2):
    return do_switch_keyboard_layout(layout1, layout2)

def do_type(type_str: str, is_switch=False, simply_type=False, long_hold=False):
    global TYPING_HISTORY
    global CURR_KBD_LAYOUT
    # import do_type first.
    find_result = find_xy(type_str)
    if simply_type and long_hold:
        exec_input('swipe {x} {y} {x} {y} 1600'.format(x=find_result[0], y=find_result[1]))
        return
    if simply_type:
        exec_input('tap {x} {y}'.format(x=find_result[0], y=find_result[1]))
        return
    if not is_switch:
        # then record it.
        TYPING_HISTORY.append({'time': get_curr_timestamp(), 'str': type_str})
        # Also: switch keyboard layout, if needed
        my_new_kbd_layout = find_result[2]
        if my_new_kbd_layout != CURR_KBD_LAYOUT:
            do_switch_keyboard_layout_decl(CURR_KBD_LAYOUT, my_new_kbd_layout)
    # then execute it.
    my_this_choice = random.randint(1, TYPING_INTERVAL_DB_LEN) - 1
    my_pressdown_time = TYPING_INTERVAL_DB[my_this_choice][1]
    my_aftersleep_time = TYPING_INTERVAL_DB[my_this_choice][0] - my_pressdown_time
    # actually execute
    exec_input('swipe {x} {y} {x} {y} {time_pressdown}'.format(
        x=find_result[0], y=find_result[1], time_pressdown=int(my_pressdown_time * 1000)))
    time.sleep(my_aftersleep_time)

# switch logic:
#
# main2sym: hit switch once
# main2upper: hit shift twice quickly
# sym2main: hit switch once
# sym2upper: (back to main first, then upper) hit switch once; hit shift twice quickly
# upper2main: hit shift once
# upper2sym: (back to main first, then sym) hit shift once; hit switch once
#
def do_switch_keyboard_layout(layout1, layout2):
    global CURR_KBD_LAYOUT
    def _hit_switch_once():
        do_type('switch', simply_type=True)
    def _hit_shift_once():
        do_type('shift', simply_type=True)      # replace with more robust impl.
    def _hit_shift_twice_quickly():
        # workaround: long hold
        do_type('shift', simply_type=True, long_hold=True)
        time.sleep(0.05)
    # Switch layout accordingly
    if layout1 == layout2:
        return
    if layout1 == 'main' and layout2 == 'symbol':
        _hit_switch_once()
    if layout1 == 'main' and layout2 == 'upper':
        _hit_shift_twice_quickly()
    if layout1 == 'symbol' and layout2 == 'main':
        _hit_switch_once()
    if layout1 == 'symbol' and layout2 == 'upper':
        _hit_switch_once()
        time.sleep(0.25)   # otherwise may enter symbol2?
        _hit_shift_twice_quickly()
    if layout1 == 'upper' and layout2 == 'main':
        _hit_shift_once()
    if layout1 == 'upper' and layout2 == 'symbol':
        _hit_shift_once()
        _hit_switch_once()
    CURR_KBD_LAYOUT = layout2
    return


def random_genstring_n(n: int):
    len_str_choice = len(STRING_CHOICE)
    ret = [STRING_CHOICE[random.randint(1, len_str_choice) - 1] for x in range(n)]
    #print('ret: {}'.format(ret))
    return ret

def clean_textfield():
    do_type('hidekbd', simply_type=True)
    time.sleep(0.5)
    exec_input('tap {} {}'.format(TARGET_LOC['textfield'][0], TARGET_LOC['textfield'][1]))
    exec_input('swipe {a} {b} {a} {b} 500'.format(a=TARGET_LOC['textfield'][0], b=TARGET_LOC['textfield'][1]))
    time.sleep(0.5)
    do_type('backspace', simply_type=True)
    time.sleep(0.5)

def exit_cleanup():
    global subp
    if subp is None:
        return
    subp.send_signal(signal.SIGINT)
    try:
        outs, errs = subp.communicate(timeout=5)
    except TimeoutExpired:
        do_log('WARNING: communicate did not finish in 5sec, killing...')
        subp.kill()
        outs, errs = subp.communicate()
    do_log('Clean exit finished!')

def real_data_collection_once():
    # really do experiment once.
    clean_textfield()
    global TYPING_HISTORY
    TYPING_HISTORY = list()
    global CURR_KBD_LAYOUT
    CURR_KBD_LAYOUT = 'main'

    # determine param for this trace
    curr_filename = Path('.') / OUTPUT_DIR_PREFIX / 'output_{}.txt'.format(int(time.time()))
    curr_pwd_length = random.randint(8, 16)
    do_log('current pwd: [{}]: {}'.format(curr_pwd_length, curr_filename))

    # get current pwd seq
    curr_pwd_seq = random_genstring_n(curr_pwd_length)
    do_log('current pwd: {}'.format(curr_pwd_seq))

    global subp
    subp = subprocess.Popen([RUNNING_BINARY_FILE], stdout=subprocess.PIPE, bufsize=5760000, close_fds=True)
    time.sleep(0.5)
    for i in curr_pwd_seq:
        # do_type includes necessary sleeping interval, and layout switch
        do_type(i[0])

    # OK. We finished typing, but give another 1.5 secs to record
    time.sleep(1.5)
    subp.send_signal(signal.SIGINT)
    try:
        outs, errs = subp.communicate(timeout=5)
    except TimeoutExpired:
        subp.kill()
        outs, errs = subp.communicate()
    subp = None

    # output: collected data first, typing history after
    with open(curr_filename, 'w') as f:
        f.write(outs.decode('UTF-8'))
        for i in TYPING_HISTORY:
            f.write('{} '.format(i['time']))
        for i in TYPING_HISTORY:
            f.write('{} '.format(i['str']))
        f.write('\n')
    do_log('End of loop...\n')


def findmatch_do_analyze():
    p = Path('./output/')
    files = list(p.glob('./findmatch_*.txt'))
    fout = open('./output/analyze.txt', 'w')
    print(files)
    for i in files:
        #print(i)
        filefreq = {}
        with open(i, 'r') as f:
            alltext = f.readlines()
        for j in alltext:
            if j == '\n':
                continue
            tmp_match = j.split(':')[2]
            try:
                filefreq[tmp_match] = filefreq[tmp_match] + 1
            except KeyError:
                filefreq[tmp_match] = 1
        list1 = filefreq.keys()
        list2 = []
        for j in list1:
            list2.append(filefreq[j])
        list2, list1 = zip(*sorted(zip(list2, list1)))
        correct_one = list1[-1].replace(';', '').strip()
        charname = str(i).split('findmatch_')[1].split('.txt')[0]
        fout.write('{} {}'.format(charname, correct_one))
    fout.close()

def findmatch():
    global subp
    for strchoice in STRING_CHOICE:
        i = strchoice
        # output filename: output/a.txt
        curr_filename = './output/findmatch_{}.txt'.format(i[0])
        subp = subprocess.Popen([RUNNING_BINARY_FILE], stdout=subprocess.PIPE, bufsize=5760000, close_fds=True)
        time.sleep(0.5)
        for j in range(REPEAT_SAMPLES):
            do_type(i[0])
        time.sleep(1.5)
        subp.send_signal(signal.SIGINT)
        try:
            outs, errs = subp.communicate(timeout=5)
        except TimeoutExpired:
            subp.kill()
            outs, errs = subp.communicate()
        subp = None
        with open(curr_filename, 'w') as f:
            f.write(outs.decode('UTF-8'))
            f.write('\n')
    ## Next, we analyze
    findmatch_do_analyze()
    ## Finish
    return


def collect_deviceinfo():
    screen_density = subprocess.run(['/system/bin/wm', 'density'], stdout=subprocess.PIPE)
    device_model = subprocess.run(['/system/bin/getprop', 'ro.product.model'], stdout=subprocess.PIPE)
    sysinfo_str = '{}__{}'.format(
            screen_density.stdout.decode('UTF-8').split('\n')[0].split(' ')[2],
            device_model.stdout.decode('UTF-8'))
    with open('./output/deviceinfo.txt', 'w') as f:
        f.write(sysinfo_str)

def randomtype():
    TOTAL_CYCLE = TOTAL_SAMPLES
    collect_deviceinfo()
    for i in range(TOTAL_CYCLE):
        real_data_collection_once()
        do_log('COUNTER: {}'.format(i))
        time.sleep(0.5)


if __name__ == '__main__':
    collect_deviceinfo()
    time.sleep(30)
    # must root
    check_is_root()
    # init first
    work_init()
    os.system('mkdir output')
    atexit.register(exit_cleanup)
    # specify working type
    if sys.argv[1] == 'findmatch':
        findmatch()
    elif sys.argv[2] == 'randomtype':
        randomtype()
