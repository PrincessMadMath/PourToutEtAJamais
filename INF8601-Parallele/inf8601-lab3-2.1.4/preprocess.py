#!/usr/bin/python
#
# Utilitaire pour pre-traiter les resultats
#

import os
import string
import pprint
import re

# variables
out_dir = "results"
base = "heatsim"
suffix = ".stats"

def natural_key(astr):
	"""See http://www.codinghorror.com/blog/archives/001018.html"""
	return [int(s) if s.isdigit() else s for s in re.split(r'(\d+)', astr)]

def get_or_create(data, key):
	if (data.has_key(key) == False):
		data[key] = {}
	return data[key]

def get_or_create_path(data, path_elem):
	if (len(path_elem) == 0):
		return data
	d = data
	for elem in path_elem:
		d = get_or_create(d, elem)
	return d

class Stat(object):
	def __init__(self):
		self.sum = 0.0
		self.cnt = 0
	def add_sample(self, item):
		self.sum += item
		self.cnt += 1
	def avg(self):
		return self.sum / self.cnt
	def __str__(self):
		return str(self.avg())
	def __repr__(self):
		return str(self.avg())

def populate_stats(data, items):
	for item in items:
		if (data.has_key(item) == False):
			data[item] = Stat()

def save_sheet(data, path):
	name = base
	for p in path:
		name += "_%s" % (p)
	name += suffix
	f = open(os.path.join(out_dir,name), "w+")
	s = "np;dimx;dimy;iter;small;medium;large;xlarge\n"
	sk = data.keys()
	sk.sort(key=natural_key)
	for k in sk:
		s += k
		stats = data[k]
		for size in ["earth-small", "earth-medium", "earth-large", "earth-xlarge"]:
			if (stats.has_key(size)):
				s += ";%.3f" % (stats[size].avg())
			else:
				s += ";0"
		s += "\n"
	f.write(s)
	f.close()

def process(filename):
	f = open(filename, "r")
	time = ["elapsed"]
	startup = {}
	compute = {}
	for l in f.readlines():
		s = l.split(";")
		img = s[0]
		np = s[1]
		dimx = s[2]
		dimy = s[3]
		iter_ = s[4]
		elapsed = s[6]
		# img,np,dimx,dimy,iter,repeat,elapsed
		data = None
		if (int(iter_) < 10):
			print "startup:" + s[5]
			data = startup
		else:
			print "compute:" + s[5]
			data = compute
		
		key = s[1:5]
		key = string.join(key, ";")
		if (data.has_key(key) == False):
			data[key] = {}
		stat = data[key]
		if (stat.has_key(img) == False):
			stat[img] = Stat()
		stat[img].add_sample(float(s[6]))
	pprint.pprint(startup)
	save_sheet(startup, ["startup"])
	pprint.pprint(compute)
	save_sheet(compute, ["compute"])
	
if __name__ == "__main__":
	filename = os.path.join(out_dir, base + suffix)
	process(filename)
