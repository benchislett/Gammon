import subprocess
import sys
import re

args = sys.argv;

datafile = "openings.txt" if len(args) <= 1 else args[1]
outputfile = "rollouts.txt" if len(args) <= 2 else args[2]
commandfile = datafile + ".instructions.cmd"

commands = "new game 1\n"

def execute(cmd):
    popen = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True)
    for stdout_line in iter(popen.stdout.readline, ""):
        yield stdout_line 
    popen.stdout.close()
    return_code = popen.wait()
    if return_code:
        raise subprocess.CalledProcessError(return_code, cmd)

lines=0
MAX_LINES = 99999999999999 # 100
with open(datafile, "r") as f:
    for line in f:
        if lines > MAX_LINES:
            break
        lines += 1

        commands += "set board " + line.strip() + "\nrollout\n"

with open(commandfile, "w") as f:
    f.write(commands)

with open(outputfile, "w") as f:
    ago = 100
    for eval_line in execute(['script', '-q', '--log-out', '/dev/null', '-c', 'gnubg -t -c ' + commandfile]):
        if 'Rollout done.' in eval_line:
            ago = 0
        else:
            ago += 1
        
        if ago == 3:
            f.write(' '.join(map(str, re.findall(r'\d+.\d+', eval_line.strip()))) + '\n')
            f.flush()

        # if '2 ply:' in eval_line:
        #     f.write(eval_line)
