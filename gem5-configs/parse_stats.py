import re
import json
from collections import defaultdict
        
# see https://stackoverflow.com/questions/16547643
def update_tree(tree, key, value):
    t = tree
    parts = key.split(".")
    for part in parts[:-1]:
        t = t.setdefault(part, {})
    t[parts[-1]] = value

def query_tree(tree, key):
    t = tree
    parts = key.split(".")
    for part in parts:
        t = t[part]
    return t

def str_to_num(num_string):
    if num_string == 'nan':
        return float('NaN')
    elif '.' in num_string:
        return float(num_string)
    else:
        return int(num_string)


def parse_stat_line(line):
    key_str, tail = line.split(' ', 1)
    if '#' in tail:
        value_str, description = tail.split('#', 1)
        value_list = value_str.split()
        description = description.lstrip().rstrip()
    else:
        value_list = tail.split()
        description = value_list.pop(-1)
    if len(value_list) > 1:
        reject_pipe_percent = lambda x: ('|' not in x) and ('%' not in x)
        value = list(filter(reject_pipe_percent, value_list))
        value = [str_to_num(s) for s in value]
    else: 
        value = str_to_num(value_list[0])
    return key_str, value, description

begin_line  = "---------- Begin Simulation Statistics ----------"
end_line    = "---------- End Simulation Statistics   ----------"

tree = {}
cur_tree = {}
cur_tree_index = 0

filename = '../m5outs/default-save/m5out-default-restore/stats.txt'
with open(filename, 'r', encoding='UTF-8') as file:
    while line := file.readline():
        if line.strip():
            line = line.rstrip()
            if line == begin_line:
                cur_tree = {}
            elif line == end_line:
                tree[cur_tree_index] = cur_tree
                cur_tree_index += 1
            else:
                # print(line)
                stat_key_str, stat_value_list, stat_description = parse_stat_line(line)
                update_tree(cur_tree, stat_key_str, (stat_value_list, stat_description))
                # if len(stat_value_list) > 1: print(stat_value_list)
                # print(cur_tree)
                # print(parse_stat_line(line))

print(query_tree(tree[10], "board.cache_hierarchy.ruby_system.m_outstandReqHistSeqr"))

print(query_tree(tree[10], "board.cache_hierarchy.ruby_system.directory_controllers0.responseFromDir.m_buf_msgs"))

print(query_tree(tree[10], "simTicks"))