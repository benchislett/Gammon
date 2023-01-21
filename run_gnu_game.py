import gnubg
import subprocess

proc = subprocess.Popen(['./interactive'], stdin=subprocess.PIPE,stdout=subprocess.PIPE)
proc.stdout.readline()

gnubg.command('new match 1')
boardhist = []

while gnubg.match()['games'][0]['info']['winner'] == None:
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

        print('Sent board to net')

        proc.stdin.write(b'swap\n')
        proc.stdin.flush()
        proc.stdout.readline()

        match = gnubg.match()
        dice = match['games'][0]['game'][-1]['dice']

        print('Got dice')

        msg = 'botroll ' + ''.join(map(str, dice)) + '\n'
        proc.stdin.write(msg.encode())
        proc.stdin.flush()

        print('Send roll to net: ', msg)

        msg = ''
        while 'Bot plays' not in msg:
                msg = proc.stdout.readline().decode()

        print('Got bot move: ', msg)
        
        botmove = msg.split('Bot plays')[1].split('with eval')[0].strip()

        gnubg.command('move ' + botmove)

        print('Sent move to bot')

gnubg.command('save match output_match.sgf')