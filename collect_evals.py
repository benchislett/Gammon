import subprocess

with open("openings.txt", "r") as f:
    for line in f:
        subprocess.run(['zsh', '../extract_evals.sh', line.strip()])
