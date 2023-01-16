split -l $((`wc -l < $1`/10)) $1 $1.split.txt -da 1
find . -type f | grep .split.txt | grep -v instructions | parallel "python3 $(git rev-parse --show-toplevel)/collect_evals.py {/} {/}.evals.txt"
rm -f evals.txt
for i in {0..9}
do
  cat $1.split.txt$i.evals.txt >> evals.txt
done
