#!/usr/bin/env python3


import os
import sys
import argparse
import random

def main(arguments):

    if(len(sys.argv) < 2):
        print("Required 2 arguments! Leaving") 
    else:
        args = sys.argv[1]
        id = sys.argv[2]
        args = args.split(":")
        op = args[0]
        prefixes = args[1]
        prefixes = prefixes.split(",")
        #print("OP: " + op)
        #print("ID: " + id)
        if op!="avr":
            sys.exit('unsupported operation: ' + op)
        temps = []
        for p in prefixes:
            if p:
                #print("prefix: "+ p)
                temps.append(random.randint(18, 25))
        avr = sum(temps)/len(temps)
        avr = str(avr)
        result = 'AVR: '+avr+'C'
        fname = id + "_out.txt"
        f = open(fname, "w")
        f.write(result)
        f.close()

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))