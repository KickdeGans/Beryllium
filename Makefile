core_files = src/*.c src/lib/*.c src/core/*.c src/runtime/*.c src/compiler/*.c
core_name = fusion
core_install_location = /bin/fusion
core_libraries_folder = /etc/fusion-lib
core_libraries_name = libraries

make:
	gcc -O3 -lm $(core_files) -o $(core_name)

clang:
	clang -O3 $(core_files) -o $(core_name)

install:
	cp $(core_name) $(core_install_location)
	sudo rm -rf $(core_libraries_folder)
	cp -r $(core_libraries_name) $(core_libraries_folder)