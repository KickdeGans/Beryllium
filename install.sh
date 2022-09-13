clear
echo "If the installer is not opened with root privileges, please cancel the installation and restart with root privileges."
echo "In order for the installer to work, make sure GCC and GIT are installed."
echo ""
read -p "Install (y/N)? " consent
if [ "$consent" = "n" ] || [ "$consent" = "N" ]; then
    exit 0
fi
echo "Select a branch to install from:"
echo "1 For latest branch"
echo "0 For release branch (recommended)"
read -p "? " branch
echo "Downloading files..."
if [ "$branch" = "1" ]; then
    git clone "https://github.com/KickdeGans/Fusion/tree/release" > /dev/null 2>&1
else
    git clone "https://github.com/KickdeGans/Fusion" > /dev/null 2>&1
fi
echo "Compiling source..."
gcc Fusion/*.c Fusion/lib/*.c -o fusion > /dev/null 2>&1
cp fusion /bin/fusion
echo "Cleaning up..."
rm -rf Fusion
rm -r ./fusion
echo "Done"
