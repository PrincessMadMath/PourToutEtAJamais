#!/usr/bin/python
#
# Utilitaire pour pre-traiter les resultats
#
import sys
import os
import glob
import re

# variables
out_dir = "results"
base = "perf-"
suffix = ".out"

header = re.compile("thread=([0-9]+),func=([a-z_]+),ht=([0-1]{1})\n")
sample = re.compile("([0-9]+)[,]+([a-zA-Z0-9_\-]+).*\n")

def get_or_create(data, key):
    if (data.has_key(key) == False):
        data[key] = {}
    return data[key]

def process(filename, out):
    f = open(filename, "r")
    # data[thread][func][counter] = val
    data = {}
    item = None
    for l in f.readlines():
        if (header.match(l)):
            m = header.match(l)
            thread = m.group(1)
            func = m.group(2)
            ht = m.group(3)
            d1 = get_or_create(data, thread)
            d2 = get_or_create(d1, ht)
            item = get_or_create(d2, func)
        elif (sample.match(l)):
            m = sample.match(l)
            value = m.group(1)
            key = m.group(2)
            item[key] = value
#        else:
#            print "no match " + l
    
    # write header
    if (len(data.keys()) == 0):
        raise IOError("wrong header")

    thread0 = data.keys()[0]
    ht0 = data[thread0].keys()[0]
    func0 = data[thread0][ht0].keys()[0]
    keys = data[thread0][ht0][func0].keys()
    keys.sort()
    out.write("thread,ht,func,")
    for key in keys:
        out.write("%s," % (key))
    out.write("\n")
        
    # write data
    for thread in data.keys():
        for ht in data[thread].keys():
            for func in data[thread][ht].keys():
                out.write("%s,%s,%s," % (thread, ht, func))
                keys = data[thread][ht][func].keys()
                keys.sort()
                for key in keys:
                    value = data[thread][ht][func][key]
                    out.write("%s," % (value))
                out.write("\n")
    

if __name__ == "__main__":
    flist = glob.glob(out_dir + "/" + base + "*" + suffix)
    for f in flist:
        out = f + ".data"
        out_file = open(out, "w")
        try:
            process(f, out_file)
            print("  OK " + f)
        except:
            print("SKIP " + f)
        
