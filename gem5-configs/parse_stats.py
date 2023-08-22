# import numpy as np
import os
from pretty_print import *

# see https://stackoverflow.com/questions/16547643
def update_tree(tree, index, key, value):
    t = tree[index]
    parts = key.split(".")
    for part in parts[:-1]:
        t = t.setdefault(part, {})
    t[parts[-1]] = value

def query_tree(tree, index, key):
    t = tree[index]
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
        # value = np.array(value)
    else: 
        value = str_to_num(value_list[0])
    return key_str, value, description

begin_line  = "---------- Begin Simulation Statistics ----------"
end_line    = "---------- End Simulation Statistics   ----------"

def parse_stats_file(filename):
    tree = {}
    tree_index = 0
    with open(filename, 'r', encoding='UTF-8') as file:
        file.flush()
        os.fsync(file.fileno())
        while line := file.readline():
            if not line.strip():
                continue
            line = line.rstrip()
            # print(line)
            if line == begin_line:
                tree[tree_index] = {}
            elif line == end_line:
                tree_index += 1
            else:
                stat_key_str, stat_value, stat_description = parse_stat_line(line)
                update_tree(tree, tree_index, stat_key_str, (stat_value, stat_description))
                # if len(stat_value_list) > 1: print(stat_value_list)
                # print(parse_stat_line(line))
    return tree

# todo: mildly confused by the histogram stuff, figure out how to interpret that ??

# blah testing

filename = '../m5outs/default-save/m5out-default-restore/stats.txt'
tree = parse_stats_file(filename)

stats_tree = parse_stats_file(filename)

print(len(stats_tree))
for region_index, region_tree in stats_tree.items():
    pretty_print(f'Region {region_index} ran in {query_tree(stats_tree, region_index, "simTicks")[0]} ticks')