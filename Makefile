install:
	gcc -Ofast -lm src/*.c src/lib/*.c -o fusion
	cp fusion /bin/fusion
	cp -r fusion-lib /bin/fusion-lib
clean:
	sudo rm fusion
	sudo rm -rf fusion-lib