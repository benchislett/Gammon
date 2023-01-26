rm -f evals_partial.txt

for i in {0..9}
do
  N=$(wc -l < $1_$i.evals.txt)
  cat $1.split.txt$i | head -n$N >> games_partial.txt
  cat $1_$i.evals.txt >> evals_partial.txt
done
