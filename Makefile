build: snake

snake: snake.c
	gcc -Wall snake.c -o snake -lcurses

run:
	./snake

clean:
	rm -f snake