#INPUT: ${FOLDER}/PREFIX.$PROCESSNUM.*
#OUTPUT: dot-formatted communication graph
import re

FOLDER = ".\\20150819"
PROCESS_NUM = 64
PREFIX = "bcast." + str(PROCESS_NUM)
COLORS = ['#ffffff','#ff8888','#88ff88','#8888ff']
result = list()

FMT = re.compile(r'\((\d*)\) (SEND|RECV)\[(dst|src)=(\d*)\]> (\d*) elapsed')

# list of (target, time, elapsed time)
# S = output file
# T = {SEND, RECV}
def parse(S, T):
    result = list()
    # Last experimental time
    for line in S[S.rfind("ENTER"):].split('\n')[1:-2]:
        (Time, Type, x, Target, Elapsed) = FMT.match(line).groups()
        if(T != Type):
            continue
        result.append((Target, Time, Elapsed))
    return result
    
for idx in range(PROCESS_NUM):
    f = open(FOLDER + "\\" + PREFIX + ".%d.log" % idx)
    S = f.read()
    result.append(parse(S, 'SEND'))

print("digraph G{")

for idx in range(PROCESS_NUM):
    print('%s[style=filled,fillcolor=\"%s\"]' % (idx, COLORS[idx//16]))

for src in range(PROCESS_NUM):    
    for dst in result[src]:
        print('%s->%s[label=%s]' % (src, dst[0], dst[2]))
        continue

print("}")
