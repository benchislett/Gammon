echo "new game 1\nset board $1\nset turn 0\nrollout" > instructions.txt
script output.txt -c "gnubg --tty -c instructions.txt"
grep "Rollout done." output.txt -A 3 | tail -n1 >> scores.txt
