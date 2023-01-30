import gnubg
import re
import subprocess

# wins = {"X": 0, "O": 0}

gnubg.command('new session')

# gnubg.command('load match output_match.sgf')
# gnubg.command('play')

gnubg.command('set cube use off')

proc = subprocess.Popen(['./interactive'], stdin=subprocess.PIPE,stdout=subprocess.PIPE)
proc.stdout.readline()

match_count = 1000000
boardhist = []

while True:
        score = gnubg.show('score')
        score = re.findall(r'\d+', score)
        score = int(score[1]) + int(score[2])
        if score >= match_count:
                break

        board = gnubg.positionid()
        proc.stdin.write(f'board {board}\n'.encode())
        proc.stdin.flush()
        proc.stdout.readline()

        if len(boardhist) >= 10 and len(set(boardhist)) == 1:
                raise Exception("Repeated board, suspect bug!")
                # print("Repeat board ", board)
        
        boardhist.append(board)
        if len(boardhist) > 10:
                boardhist.pop(0)

        proc.stdin.write(b'swap\n')
        proc.stdin.flush()
        proc.stdout.readline()

        dice = gnubg.show('dice')
        if 'gnubg' in dice:
                gnubg.command('accept')
                gnubg.command('save match output_match.sgf')
                continue

        assert ('gnubg' not in dice)

        dice = dice[dice.index('rolled')+7:-1]
        dice = dice[0] + dice[-1]

        msg = 'botroll ' + dice + '\n'
        proc.stdin.write(msg.encode())
        proc.stdin.flush()

        msg = ''
        while 'Bot plays' not in msg:
                msg = proc.stdout.readline().decode()
        
        botmove = msg.split('Bot plays')[1].split('with eval')[0].strip()

        gnubg.command('move ' + botmove)

gnubg.command('save match output_match.sgf')

