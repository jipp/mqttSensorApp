import subprocess

revision = subprocess.check_output(["git", "describe", "--tag", "--always", "--dirty"]).strip()
print "-D VERSION='\"%s\"'" % revision
