<img src="Beryllium-Logo.png" width="256"></img>
# beryllium
## Version 0.5<br>

Example:
```beryllium
fun main()
{
    puts("Hello world!\n");
}
```

### Quick Install:
```shell
curl -s "https://www.kickdegans.nl/beryllium-install" | sudo bash
```

## An interpretted programming language

## Contents:
[Getting started](https://github.com/KickdeGans/beryllium/wiki)
<br>
[Documentation](https://github.com/KickdeGans/beryllium/wiki/Documentation)
<br>
[Bult-in functions](https://github.com/KickdeGans/beryllium/wiki/Built-in-functions)
<br>
### Please read!
This projects is still in development and contains incomplete or non functional features, bugs and incomplete libraries.<br>
If there are any bugs you find, please notify me.<br>

### Installation:
#### Clone the repo:
```shell
git clone https://www.github.com/KickdeGans/beryllium.git
```
#### Build and install using GCC:
```shell
make
make install
```
#### Build and install using clang:
```shell
make clang
make install
```

#### OR:
You could copy the correct pre-compiled binary to /bin if processing power is limited or no compiler is present.<br><br>
For example, you could copy ```berylium-x86_64``` (located in the ```bin``` folder) to /bin or just execute it as-is to run the interpreter.<br>
(More architectures will be available in the future)<br>

### Debugging:
Change directory to the debug folder.
Then, make and then run ```./debug``` to debug beryllium.
Make sure the script has permissions to be executed
#### Requirements:
```Text
Linux
gdb
gcc
```
