#!bin/bash
git clone "https://github.com/KickdeGans/Fusion"
gcc *.c lib/*.c -o fusion
cp fusion /bin/fusion