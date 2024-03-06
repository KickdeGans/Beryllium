core_files = src/*.c src/lib/*.c src/core/*.c src/runtime/*.c src/compiler/*.c
core_name = beryllium
core_install_location = /bin/beryllium
core_libraries_folder = /etc/beryllium-lib
core_libraries_name = libraries
compile_flags = -O3 -lm

make:
	gcc $(compile_flags) $(core_files) -o $(core_name)

clang:
	clang $(compile_flags) $(core_files) -o $(core_name)

install:
	cp $(core_name) $(core_install_location)
	cp -r $(core_libraries_name) $(core_libraries_folder)