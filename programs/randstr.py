#!/usr/bin/python
import random
import sys;

for i in range(int(sys.argv[1])):
  if (random.random() >= .5):
    sys.stdout.write('a')#print "a" ,
  else:
    sys.stdout.write('b')#print "b" ,
print ""
