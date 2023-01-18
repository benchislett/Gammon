import subprocess
import sys

args = sys.argv;

datafile = "openings.txt" if len(args) <= 1 else args[1]
outputfile = "evals.txt" if len(args) <= 2 else args[2]
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

        commands += "set board " + line.strip() + "\neval\n"

with open(commandfile, "w") as f:
    f.write(commands)

with open(outputfile, "w") as f:
    for eval_line in execute(['gnubg', '-t', '-c', commandfile]):
        if '2 ply:' in eval_line:
            f.write(eval_line)
