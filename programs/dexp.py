#!/usr/bin/python

def dexp_re(n):
  return "(a|b)*a" + ("(a|b)"*n)

def dexp_str(n):
  return "a"*n

import re;
import sys;
#print "^"+dexp_re(int(sys.argv[1]))+"$"
#print dexp_str(int(sys.argv[2]))
print re.match("^"+dexp_re(int(sys.argv[1]))+"$",dexp_str(int(sys.argv[2])))

