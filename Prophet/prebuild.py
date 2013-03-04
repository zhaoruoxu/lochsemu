import sys

FileName = 'buildver.h'

def main(argv):
	f = open(FileName, 'r')
	line = f.readline()
	ss = line.split()
	f.close()

	ver = int(ss[2])
	ver += 1
	ss[2] = str(ver)

	line = ' '.join(ss)
	f = open(FileName, 'w')
	f.write(line)
	f.close()


if __name__ == '__main__':
	main(sys.argv)