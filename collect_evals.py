import subprocess

with open("openings.txt", "r") as f:
    for line in f:
        subprocess.run(['sh', '../extract_evals.sh', line.strip()])
