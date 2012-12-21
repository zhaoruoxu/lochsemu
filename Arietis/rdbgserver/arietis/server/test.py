'''
Created on 2012-12-21

@author: Administrator
'''

import socket
from arietis.common import VERSION

BUFFER_SIZE     = 4096
MSG_TERMINATOR  = '\n\n'

def Recv(conn):
    msg = ''
    while True:
        buf = conn.recv(BUFFER_SIZE)
        if buf == '\n\n':
            raise RuntimeError('conn.rect() failed')
        termPos = buf.find(MSG_TERMINATOR)
        if termPos == -1:
            msg += buf
            continue
        msg += buf[:termPos]
        break
    # conn.send(b'okay')
    return msg

def Send(conn, msg):
    msg += MSG_TERMINATOR
    l = len(msg)
    totalSent = 0
    while totalSent < l:
        sent = conn.send(msg[totalSent:])
        if sent == 0:
            raise RuntimeError('conn.send() failed')
        totalSent += sent
    
def StartServer():
    print('Arietis Remote Debugging Server, version %x' % (VERSION))
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('localhost', 7777))
    sock.listen(5)
    
    print('Waiting connections...')
    
    while True:
        conn, address = sock.accept()
        print('client accepted:', address)
        try:
            # conn.settimeout(5)
            msg = Recv(conn)
            print(msg)
            # do something here
            Send(conn, 'hello')
            
            while True:
                msg = Recv(conn)
                print(msg)
                if msg[:3] == 'bye':
                    print('client goodbye')
                    break
        #except ConnectionResetError:
        #    print('Connection reset')
        except socket.timeout:
            print('timed out')
        conn.close()
    