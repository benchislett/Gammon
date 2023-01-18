split -l $((`wc -l < $1`/10)) $1 $1.split.txt -da 1
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt0 $1_0.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt1 $1_1.evals.txt & 
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt2 $1_2.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt3 $1_3.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt4 $1_4.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt5 $1_5.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt6 $1_6.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt7 $1_7.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt8 $1_8.evals.txt &
python3 $(git rev-parse --show-toplevel)/collect_evals.py $1.split.txt9 $1_9.evals.txt &

wait

rm -f evals.txt
for i in {0..9}
do
  cat $1_$i.evals.txt >> evals.txt
done
