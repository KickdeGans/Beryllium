echo "downloading files..."
git clone "https://github.com/KickdeGans/Fusion" > dev/null
echo "compiling source..."
gcc Fusion/*.c Fusion/lib/*.c -o fusion > dev/null
cp fusion /bin/fusion
echo "done"