import subprocess

with open("games.txt", "r") as f:
    for line in f:
        subprocess.run(['zsh', '../extract_rollout.sh', line.strip()])