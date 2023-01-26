import gnubg
import subprocess

wins = {"X": 0, "O": 0}


gnubg.command('new session')
gnubg.command('set cube use off')

for i in range(100):
        proc = subprocess.Popen(['./interactive'], stdin=subprocess.PIPE,stdout=subprocess.PIPE)
        proc.stdout.readline()

        boardhist = []

        while gnubg.match()['games'][i]['info']['winner'] == None:
                board = gnubg.positionid()
                proc.stdin.write(f'board {board}\n'.encode())
                proc.stdin.flush()
                proc.stdout.readline()

                if len(boardhist) == 5 and len(set(boardhist)) == 1:
                        gnubg.command('accept')
                        # print("Repeat board ", board)
                        break
                
                boardhist.append(board)
                if len(boardhist) > 5:
                        boardhist.pop(0)

                proc.stdin.write(b'swap\n')
                proc.stdin.flush()
                proc.stdout.readline()

                match = gnubg.match()
                dice = match['games'][i]['game'][-1]['dice']

                msg = 'botroll ' + ''.join(map(str, dice)) + '\n'
                proc.stdin.write(msg.encode())
                proc.stdin.flush()

                msg = ''
                while 'Bot plays' not in msg:
                        msg = proc.stdout.readline().decode()
                
                botmove = msg.split('Bot plays')[1].split('with eval')[0].strip()

                gnubg.command('move ' + botmove)

        winner = gnubg.match()['games'][i]['info']['winner']
        if winner:
                wins[winner] += 1

        proc.kill()

gnubg.command('save match output_match.sgf')

print(wins)
