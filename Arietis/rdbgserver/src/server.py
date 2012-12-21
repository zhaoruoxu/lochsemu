'''
Created on 2012-12-21

@author: Administrator
'''

import sys
import socket

VERSION = 0x20121221

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
            buf = conn.recv(1024)
            print(str(buf[:buf.find(b'\n')], 'ascii'))
            conn.send(b'hello')
            
            while True:
                buf = conn.recv(1024)
                s = str(buf[:buf.find(b'\n')], 'ascii')
                print(s)
                if s[:4] == b'bye\n':
                    print('client goodbye')
                    break
                if len(buf) == 0:
                    print('shit happens')
                    break
                
        except socket.timeout:
            print('timed out')
        conn.close()
    

if __name__ == '__main__':
    main(sys.argv)