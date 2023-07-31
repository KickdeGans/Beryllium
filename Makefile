core_files = src/*.c src/lib/*.c src/core/*.c src/runtime/*.c src/compiler/*.c
packager_files = fusion-packages/main.c
core_name = fusion
packager_name = fusion-packages/fusion-packages
core_install_location = /bin/fusion
packager_install_location = /bin/fusion-packages
core_libraries_folder = /etc/fusion-lib
core_libraries_name = fusion-lib

make:
	gcc -O3 -lm $(core_files) -o $(core_name)
	gcc $(packager_files) -o $(packager_name)

clang:
	clang -O3 $(core_files) -o $(core_name)
	clang $(packager_files) -o $(packager_name)

install:
	cp $(core_name) $(core_install_location)
	sudo rm -rf $(core_libraries_folder)
	cp -r $(core_libraries_name) $(core_libraries_folder)

	cp $(packager_name) $(packager_install_location)

debug:
	gcc -Wall -g3 -Werror -O3 -lm $(core_files) -o fusion.debug