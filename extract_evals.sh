echo "new game 1\nset board $1\nset turn 0\neval" > instructions.txt
gnubg --tty -c instructions.txt > output.txt 2> /dev/null
grep "Evaluator" output.txt -A 6 | tail -n1 >> evals.txt
