#!/usr/bin/env python3
#	eprof.py
#	Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

import os, sys, math, re

#	parse attributes into a string (hopefully of 9 char)

def cnt_str(cnt):	
	if cnt == 0:
		return '		 '
	else:
		return f'{cnt:9d}'

### Open the call / ret helper
callret_f = open("../core_call.log")

calls = {}
rets  = {}

for l in callret_f:
	# The format is "calling_addr dst_addr clk ret/call
	calling_addr = int('0x'+l[0:8], 16)
	dst_addr = int('0x'+l[9:9+8], 16)
	clk_cycles = int(l[9+8+1:9+8+1+10])
	call_type = l[9+8+1+10+1:-1]
	if call_type == "call":
		calls[calling_addr] = (dst_addr, clk_cycles)
	else:
		rets[calling_addr] = (dst_addr, clk_cycles)

callret_f.close()

### First pass
#   find all functions addresses
fr = open("../firmware.pmap")

syms = {}	   #	Functions addresses
addr = 0
new_addr = False

for sf in fr:
	s = sf.rstrip()
	if s[0:2] == '0x':
		addr = int(s,16)
		new_addr = True
	elif s.find(':') > 0:
		new_addr = False
	else:
		# Same address as the last one for a different symbol? pass as this means
		# that we are parsing an inline function
		if not new_addr:
			continue
		# Check if this is a new function name
		if s not in syms:
			syms[s] = [addr]
		else:
			if addr not in syms[s]:
				# New address to add
				syms[s].append(addr)
		new_addr = False

fr.close()
print(syms)

### Second pass
#   find all functions call sites
fr = open("../firmware.pmap")

tmp_calls = []
last_addr = addr = 0
last_s = None
new_addr = False

all_syms_addr = {}
for f in syms:
	for a in syms[f]:
		if a in all_syms_addr:
			all_syms_addr[a].append(f)
		else:
			all_syms_addr[a] = [f]

for sf in fr:
	s = sf.rstrip()
	if s[0:2] == '0x':
		last_addr = addr
		addr = int(s,16)
		new_addr = True
	elif s.find(':') > 0:
		new_addr = False
	else: 
		# For all function symbols, find if this is a call
		if not new_addr:
			continue
		if f in all_syms_addr[addr]:
			tmp_calls.append((last_s, f, last_addr, addr))
			break
		last_s = s

fr.close()

# Build the call graph from all the calling sites
# The recursive functions are unfortunately not accounted here ...
call_graph = {}
for (a, b, c, d) in tmp_calls:
	if a in call_graph:
		if ((b, c) not in call_graph[a]) and (a != b) and (c not in rets):
			call_graph[a].append((b, c))
	elif (a != b) and (c not in rets):
		call_graph[a] = [(b, c)]

### Third pass
#	read source file mapping
fsc = {}		#	function counts
lsc = {}		#	source line counts

fr = open("../firmware.pmap")
addr = 0x0

for sf in fr:
	s = sf.rstrip()
	if s[0:2] == '0x':
		addr = int(s,16)
	elif s.find(':') > 0:
		v = s.split(':')
		if v[1][0].isdigit():
			ln = int(v[1].split()[0])
			fn = v[0]
			if fn in lsc:
				if ln in lsc[fn]:
					lsc[fn][ln] = lsc[fn][ln] + 1
				else:
					lsc[fn][ln] = 1
			else:
				lsc[fn] = {ln: 1}
	else:
		if s in fsc:
			fsc[s] = fsc[s] + 1
		else:
			fsc[s] = 1
fr.close()

#print(fsc)
#print("======\n")
#print(lsc)

#	breakdown by function

fw = open("func.txt", "w")
for fc in fsc:
	fw.write(f'{cnt_str(fsc[fc])} : {fc}\n')
fw.close()

#	individual files

for rfn in lsc:
	pfl=len(os.path.commonprefix([os.getcwd(), rfn]))
	wfn = rfn[pfl:].replace("/","_");
	try:
		# Try to open the source file, if not possible skip it
		fr = open(rfn, "r")
	except:
		print("[-] Skipping %s (not found)" % rfn)
		continue
	print("writing:", wfn)
	fw = open(wfn, "w")
	ln = 0
	for sf in fr:
		ln += 1
		s = sf.rstrip()
		if ln in lsc[rfn]:
			fw.write(f'{cnt_str(lsc[rfn][ln])} : {s}\n')
		else:
			fw.write(f'		  : {s}\n')
	fr.close()
	fw.close()

