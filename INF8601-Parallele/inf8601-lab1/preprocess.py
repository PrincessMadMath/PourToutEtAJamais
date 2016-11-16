#!/usr/bin/python
#
# Utilitaire pour pre-traiter les resultats
#

import os

# variables
out_dir = "results"
base = "time_dragonizer"
suffix = ".data"

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

def save_sheet(s, path):
	name = base
	for p in path:
		name += "_%s" % (p)
	name += suffix
	f = open(os.path.join(out_dir,name), "w+")
	f.write(s)
	f.close()

def process(filename):
	f = open(filename, "r")
	time = ["sys", "user", "elapsed"]
	data = {}
	for l in f.readlines():
		s = l.split(",")
		# cmd,lib,pwr,thd,sys,user,elapsed
		offset = 4
		e = get_or_create_path(data, s[0:offset])
		populate_stats(e, time)
		for i,t in enumerate(time):
			e[t].add_sample(float(s[i+offset]))
		
	
	# output files
	for k1 in data.keys():
		d1 = data[k1]
		for k2 in d1.keys():
			d2 = d1[k2]
			for k3 in d2.keys():
				d3 = d2[k3]
				s = ""
				s += "cmd=%s lib=%s power=%s\n" % (k1, k2, k3)
				s += "threads,sys,user,elapsed\n"
				kset = d3.keys()
				kset.sort()
				for thd in kset:
					#print d3[thd]["sys"]
					d4 = d3[thd]
					s += "%s,%.3f,%.3f,%.3f\n" % (thd, d4["sys"].avg(), d4["user"].avg(), d4["elapsed"].avg())
				save_sheet(s, [k1,k2,k3]);

if __name__ == "__main__":
	filename = os.path.join(out_dir, base + suffix)
	process(filename)