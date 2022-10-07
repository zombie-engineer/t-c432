#!/bin/python3
import subprocess


def main():
  cmd = ['./run_openocd.sh']
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE, universal_newlines=True)
  for stdout_line in iter(p.stdout.readline, ""):
    yield stdout_line

  popen.stdout.close()

main()
