#!/usr/bin/env python
from __future__ import print_function

# args: visualize_comm_trace.py [process_number] 
#INPUT: ${FOLDER}/PREFIX.$PROCESSNUM.*
#OUTPUT: dot-formatted communication graph(s)

import re
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-n", "--number-processes", help="the number of processes of the experimental test case", required=True)
parser.add_argument("-ppn", "--processes-per-node", help="the number of processes per node of the test case", required=True, type=int)
parser.add_argument("-t", "--type", help="type of trace to parse: [bcast/alltoall]", required=True)

args = parser.parse_args()

_PROCESS_NUM = args.number_processes
_PROCESSES_PER_NODE = args.processes_per_node
_TYPE = args.type
_SUPPORTED_TYPE = ['bcast', 'alltoall']

if not _PROCESS_NUM:
	_PROCESS_NUM = raw_input('INPUT PROCESS NUMBER(DEFAULT=64): ')

if not _TYPE in _SUPPORTED_TYPE:
    raise Exception('Unsupported type: ' + _TYPE)

FOLDER = "."
PROCESS_NUM = int(_PROCESS_NUM) if len(_PROCESS_NUM)>0 else 64
PREFIX = "%s.%s" % (_TYPE, str(PROCESS_NUM))
COLORS = ['#ffffff','#ff8888','#88ff88','#8888ff']
result = list()

# list of (target, time, elapsed time)
# S = output file as str
# T = {MPI_SEND, MPI_RECV}
def parse_bcast(S, T):
    result = list()
    # Last experimental time
    for line in S[S.rfind("ENTER_ITEM"):].split('\n')[1:-1]:
        (Time, Elapsed, Type, Target) = line.split()
        if(T != Type):
            continue
        result.append((Target, Time, Elapsed))
    return result

# list of (src, target, time, elapsed time)
# S = output file as str
# T = {MPI_SEND, MPI_RECV}
def parse_alltoall(S):
    result = list()
    # Last experimental time
    for line in S[S.rfind("ENTER_ITEM"):].split('\n')[1:-1]:
        (Time, Elapsed, Type, Src, Target) = line.split()
        if(Type != 'MPI_SENDRECV'):
            continue
        result.append((Src, Target, Time, Elapsed))
    return result
    
for idx in range(PROCESS_NUM):
    f = open(FOLDER + "/" + PREFIX + ".%d.log" % idx)
    S = f.read()
    if _TYPE == 'bcast':
        result.append(parse_bcast(S, 'MPI_SEND'))
    elif _TYPE == 'alltoall':
        result.append(parse_alltoall(S))

if _TYPE == 'bcast':
    print("digraph G{")
    for idx in range(PROCESS_NUM):
        print('%s[style=filled,fillcolor=\"%s\"]' % (idx, COLORS[idx//_PROCESSES_PER_NODE]))
    for src in range(PROCESS_NUM):    
        for dst in result[src]:
            print('%s->%s[label=%s]' % (src, dst[0], dst[2]))
            continue
    print("}")
elif _TYPE == 'alltoall':
    for i in range(PROCESS_NUM - 1):
        with open('alltoall.%d.dot' % i, 'w') as f:
            print("digraph G{", file=f)
            for idx in range(PROCESS_NUM):
                print('%s[style=filled,fillcolor=\"%s\"]' % (idx, COLORS[idx//_PROCESSES_PER_NODE]), file=f)
            for src in range(PROCESS_NUM):
                print('%s->%s[label=%s]' % (src, result[src][i][1], result[src][i][3]), file=f)
            print("}", file=f)

