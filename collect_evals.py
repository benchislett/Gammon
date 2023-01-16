import subprocess
import sys

args = sys.argv;

datafile = "openings.txt" if len(args) <= 1 else args[1]
outputfile = "evals.txt" if len(args) <= 2 else args[2]
commandfile = datafile + ".instructions.cmd"

commands = "new game 1\n"

lines=0
MAX_LINES = 9999999999 # 100
with open(datafile, "r") as f:
    for line in f:
        if lines > MAX_LINES:
            break
        lines += 1

        commands += "set board " + line.strip() + "\neval\n"

with open(commandfile, "w") as f:
    f.write(commands)

result = subprocess.run(['gnubg', '-t', '-c', commandfile], stdout=subprocess.PIPE)
result = '\n'.join(filter(lambda s: '2 ply:' in s, result.stdout.decode().split('\n')))

with open(outputfile, "w") as f:
    f.write(result)

