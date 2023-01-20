make:
	gcc -Ofast -lm src/*.c src/lib/*.c -o fusion
	gcc fusion-packages/main.c -o fusion-packages/fusion-packages

install:
	cp fusion /bin/fusion
	sudo rm -rf /bin/fusion-lib
	cp -r fusion-lib /bin/fusion-lib

	cp fusion-packages/fusion-packages /bin/fusion-packages

dev:
	gcc -Wall -Werror -Ofast -lm src/*.c src/lib/*.c -o fusion