#!/usr/bin/python

def cox_re(n):
  if n<=0:
    return ""
  else:
    return "a?" + cox_re(n-1) + "a"

def cox_str(n):
  if n<=0:
    return ""
  else:
    return "a" + cox_str(n-1)

import re;
import sys;
print re.match(cox_re(int(sys.argv[1])),cox_str(int(sys.argv[1])))
