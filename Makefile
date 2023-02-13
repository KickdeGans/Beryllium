core_files = src/*.c src/lib/*.c src/core/*.c src/runtime/*.c src/compiler/*.c
packager_files = fusion-packages/main.c
core_name = fusion
packager_name = fusion-packages/fusion-packages
core_install_location = /bin/fusion
packager_install_location = /bin/fusion_packages
core_libraries_folder = /bin/fusion-lib
core_libraries_name = fusion-lib

make:
	gcc -Ofast -lm $(core_files) -o $(core_name)
	gcc $(packager_files) -o $(packager_name)

install:
	cp $(core_name) $(core_install_location)
	sudo rm -rf $(core_libraries_folder)
	cp -r $(core_libraries_name) $(core_libraries_folder)

	cp $(packager_name) $(packager_install_location)

dev:
	gcc -Wall -ggdb -g -Werror -Ofast -lm $(core_files) -o $(core_name)