import shutil
def setup(src):
    shutil.copyfile(src, "/bin")
setup("./fusion")
print("Setup complete")
