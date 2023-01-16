import subprocess

commands = "new game 1\n"

lines=0
with open("openings.txt", "r") as f:
    for line in f:
        if lines > 1000:
            break
        lines += 1

        commands += "set board " + line.strip() + "\neval\n"

with open("instructions.txt", "w") as f:
    f.write(commands)

result = subprocess.run(['gnubg', '-t', '-c', 'instructions.txt'], stdout=subprocess.PIPE)
result = '\n'.join(filter(lambda s: '2 ply:' in s, result.stdout.decode().split('\n')))

with open("evals.txt", "w") as f:
    f.write(result)

