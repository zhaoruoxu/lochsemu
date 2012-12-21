'''
Created on 2012-12-21

@author: Administrator
'''

import sys
import socket

VERSION         = 0x20121221
BUFFER_SIZE     = 4096
MSG_TERMINATOR  = b'\n\n'

def Recv(conn):
    msg = b''
    while True:
        buf = conn.recv(BUFFER_SIZE)
        if buf == b'\n\n':
            raise RuntimeError('conn.rect() failed')
        termPos = buf.find(MSG_TERMINATOR)
        if termPos == -1:
            msg += buf
            continue
        msg += buf[:termPos]
        break
    # conn.send(b'okay')
    return str(msg, 'ascii')

def Send(conn, msg):
    msg += MSG_TERMINATOR
    l = len(msg)
    totalSent = 0
    while totalSent < l:
        sent = conn.send(msg[totalSent:])
        if sent == 0:
            raise RuntimeError('conn.send() failed')
        totalSent += sent
    
def main(argv):
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
            Send(conn, b'hello')
            
            while True:
                msg = Recv(conn)
                print(msg)
                if msg[:3] == 'bye':
                    print('client goodbye')
                    break
        except ConnectionResetError:
            print('Connection reset')
        except socket.timeout:
            print('timed out')
        conn.close()
    

if __name__ == '__main__':
    main(sys.argv)