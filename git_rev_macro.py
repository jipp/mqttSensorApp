import subprocess

revision = subprocess.check_output(["git", "describe", "--tag", "--always", "--dirty"]).strip().decode()
print("-D VERSION='\"%s\"'" % revision)
