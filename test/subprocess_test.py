import subprocess

cpp = subprocess.Popen("", executable="main.exe", stdin=subprocess.PIPE, stdout=subprocess.PIPE, universal_newlines=True, bufsize=1)
while cpp.poll() == None:
    print(cpp.stdout.readline())
cpp.terminate()