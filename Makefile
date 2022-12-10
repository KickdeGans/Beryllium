install:
	gcc -Ofast -lm src/*.c src/lib/*.c -o fusion
	cp fusion /bin/fusion
	sudo rm -rf /bin/fusion-lib
	cp -r fusion-lib /bin/fusion-lib