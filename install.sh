read -p "install (y/n)? " consent
if [ "$consent" = "n" ] || [ "$consent" = "N" ]; then
    exit 0
fi
echo "download from release branch or latest branch"
echo "1 for latest branch"
echo "0 for release branch"
read -p "? " branch
echo "downloading files..."
if [ "$branch" = "1" ]; then
    git clone "https://github.com/KickdeGans/Fusion/tree/release" > /dev/null 2>&1
else
    git clone "https://github.com/KickdeGans/Fusion" > /dev/null 2>&1
fi
echo "compiling source..."
gcc Fusion/*.c Fusion/lib/*.c -o fusion > /dev/null 2>&1
cp fusion /bin/fusion
echo "cleaning up..."
rm -rf Fusion
rm -r ./fusion
echo "done"
