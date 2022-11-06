install:
	gcc -Ofast -lm *.c lib/*.c -o fusion
	cp fusion /bin/fusion
	rm -rf Fusion
	rm -r ./fusion