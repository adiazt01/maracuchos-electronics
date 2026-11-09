CFLAGS = -std=c11 -Wall -Wextra -O2

main: main.c
	gcc $(CFLAGS) -o main main.c

test:
	python3 -m unittest test_main

tiempo: main
	./main tiempo
	python3 main.py tiempo

.PHONY: test tiempo
