all: logger acmonitor test_aclog

logger.so: logger.c
	gcc -Wall -fPIC -shared -o logger.so logger.c -lcrypto -ldl


acmonitor: acmonitor.c
	gcc -o acmonitor acmonitor.c

test_aclog: test_aclog.c
	gcc -o test_aclog test_aclog.c

run: logger.so test_aclog
	LD_PRELOAD=./logger.so ./test_aclog

clean:
	rm -rf logger.so
	rm -rf test_aclog
	rm -rf acmonitor
