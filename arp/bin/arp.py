#!/usr/bin/python -O

versionstring = "beta3"
datestring = "june 30, 2006"

import os,sys,stat,re
import arp_help, arp_output, arp_utils

from arp_output import ctext


actions=[
"pack", "unpack", "list",
"delete",   "make",   "test",
"help"
]

options=["--tree", "--version", "--pretend","--help"]
shortmapping={"P":"--pack", "U":"--unpack", "L":"--list", 
              "D":"--delete", "M":"--make", "T":"--test",
              "h":"--help","h":"--help",
              "t":"--tree","v":"--version","p":"--pretend"}

myaction=None
myopts=[]
myfiles=[]
edebug=0

# process short actions
tmpcmdline=sys.argv[1:]
cmdline=[]

if tmpcmdline==[]:
  arp_help.shorthelp()
  sys.exit(0)

for x in tmpcmdline:
	if x[0:1]=="-"and x[1:2]!="-":
		for y in x[1:]:
			if shortmapping.has_key(y):
				if shortmapping[y] in cmdline:
					print
					print "*** Warning: Redundant use of",shortmapping[y]
				else:
					cmdline.append(shortmapping[y])
			else:
				print "!!! Error: -"+y+" is an invalid short action."
				sys.exit(1)
	else:
		cmdline.append(x)

# process the options and command arguments
for x in cmdline:
	if not x:
		continue
	if len(x)>=2 and x[0:2]=="--":
		if x in options:
			myopts.append(x)
		elif x[2:] in actions:
			if myaction:
				if myaction not in ["system", "world"]:
					myaction="--"+myaction
				print
				print ctext(['red'],"!!!")+ctext(['green'],\
				" Multiple actions requested... Please choose one only.")
				if myaction != x:
				  print ctext(['red'],"!!!")+" '"+\
				  ctext(['darkgreen'],myaction)+"' "+\
				  ctext(['red'],"or")+" '"+ctext(['darkgreen'],x)+"'"
				print
				sys.exit(1)
			myaction=x[2:]
		else:
			print "!!! Error:",x,"is an invalid option."
			sys.exit(1)
	elif x[-1]=="/":
		# this little conditional helps tab completion
		myfiles.append(x[:-1])
	else:
		myfiles.append(x)

if ("--help" in myopts) or (myaction=="help") or (not myaction):
  arp_help.shorthelp();
  sys.exit(0)

if not myfiles:
  print ctext(['red'],"!!! Error: please specify a target")
  sys.exit(1)

# print "OPTIONS:",
# for x in myopts:
#   print x,
# print
# print "ACTION:",
# print myaction
# print "FILES:",
# for x in myfiles:
#   print x,
# print


mypath = os.getcwd()
dirlist = []
if ("all" in myfiles):
  tmpdirlist = os.listdir(mypath+"/platforms")
  for x in tmpdirlist:
    tmpstat = os.lstat(mypath+"/platforms/"+x)
    if stat.S_ISDIR(tmpstat[stat.ST_MODE]):
      if ("svn" not in x):
        dirlist.append(x)
elif "unpack" not in myaction:
  for x in myfiles:
    try:
      tmpstat = os.lstat(mypath+"/platforms/"+x)
    except:
      try:
        tmpstat = os.lstat(mypath+"/"+x)
      except:
        print ctext(['red'],"!!! Error: cannot access "+x)
        sys.exit(1)
    if stat.S_ISDIR(tmpstat[stat.ST_MODE]):
      if ("svn" not in x):
        dirlist.append(os.path.normpath(x))
    elif stat.S_ISREG(tmpstat[stat.ST_MODE]):
      print ctext(['red'],"!!! Error: argument must be a platform ("+x+")")
      sys.exit(1)
    else:
      print ctext(['red'],"!!! Error: please verify "+x+" (seems a link)")
      sys.exit(1)




if ("--tree" in myopts) and ( "list" != myaction):
  print ctext(['red'],"!!! Error: option --tree only makes sense with list action.")
  sys.exit(1)


if "unpack" in myaction:
  for x in myfiles:
    if not os.path.exists(x): 
      print ctext(['red'],"!!! Error: package "+x+" doesn't exists.")
    else:
      print "Unpacking platform "+x+"..."
      cmd = "tar xmzf "+x
      cstdin, cstdout, cstderr = os.popen3(cmd)
      cstdin.close()
      errors = cstderr.readlines()
      if errors: 
	for y in errors:
	  print "-- "+y[5:],
	print ctext(['red','blink'],"There are errors.")
      cstderr.close()
      cstdout.close()
      print "Done."

elif "pack" in myaction:
  print "PACK"
  for x in dirlist:
    if not os.path.exists("platforms/"+x+"/defs.arp"):
      if os.path.exists(x):
	head,tail=os.path.split(x)
	print "Packing "+tail+"..."
	cmd = "tar cz --exclude='*.svn*' -f "+tail+".arppack "+x
	cstdin, cstdout, cstderr = os.popen3(cmd)
	cstdin.close()
	errors = cstderr.readlines()
	if errors:
	  for y in errors:
	    print "-- "+y[5:],
	  print ctext(['red','blink'],"There are errors. Your package will be incomplete.")
	cstderr.close()
	cstdout.close()
      else:
        print ctext(['red'],"!!! Error: "+x+" doesn't exists (defs.arp missing?).")
    else:
      platname,platstruct = arp_utils.getdefstructure("platforms/"+x+"/defs.arp")
      print "Packing platform "+x+"..."
      cmd = "tar cz --exclude='*.svn*' -f "+x+".arppack "+" ".join(platstruct) +" platforms/"+x
      cstdin, cstdout, cstderr = os.popen3(cmd)
      cstdin.close()
      errors = cstderr.readlines()
      if errors:
	for y in errors:
	  print "-- "+y[5:],
	print ctext(['red','blink'],"There are errors. Your package will be incomplete.")
      cstderr.close()
      cstdout.close()
      print "Done."


elif "list" in myaction:
  for x in dirlist:
    if not os.path.exists("platforms/"+x+"/defs.arp"): 
      print ctext(['red'],"!!! Error: platforms/"+x+"/defs.arp doesn't exists.")
    else:
      platname,platstruct = arp_utils.getdefstructure("platforms/"+x+"/defs.arp")
      print "PLATAFORM "+x+" is:"
      for y in platstruct:
	print " -- "+y
      print "---"

elif "delete" in myaction:
  gooddirlist=[]
  goodlist=[]
  tmpdirlist = os.listdir(mypath+"/platforms")
  for x in tmpdirlist:
    tmpstat = os.lstat(mypath+"/platforms/"+x)
    if stat.S_ISDIR(tmpstat[stat.ST_MODE]):
      if (("svn" not in x) and (x not in dirlist)):
	if os.path.exists("platforms/"+x+"/defs.arp"):
          gooddirlist.append(x)
  for x in gooddirlist:
    platname,platstruct = arp_utils.getdefstructure("platforms/"+x+"/defs.arp")
    for y in platstruct:
      if y not in goodlist:
	goodlist.append(y)
  print "DELETE"
  for x in dirlist:
    if not os.path.exists("platforms/"+x+"/defs.arp"):
      if os.path.exists(x):
	head,tail=os.path.split(x)
	print "Deleting "+tail+"..."
	cmd = "rm -Rf "+x
	cstdin, cstdout, cstderr = os.popen3(cmd)
	cstdin.close()
	errors = cstderr.readlines()
	if errors:
	  for y in errors:
	    print "-- "+y[5:],
	  print ctext(['red','blink'],"There are errors.")
	cstderr.close()
	cstdout.close()
      else:
        print ctext(['red'],"!!! Error: "+x+" doesn't exists (defs.arp missing?).")
    else:
      platname,platstruct = arp_utils.getdefstructure("platforms/"+x+"/defs.arp")
      print "Deleting platform "+x+"..."
      for y in platstruct:
	if y in goodlist:
	  print "ARP: "+y+" is part of another platform and will not be removed automatically."
	  print "ARP: Use arp --delete "+y+" to do it."
	  platstruct.remove(y)
      
      cmd = "rm -Rf "+" ".join(platstruct) +" platforms/"+x
      cstdin, cstdout, cstderr = os.popen3(cmd)
      cstdin.close()
      errors = cstderr.readlines()
      if errors:
	for y in errors:
	  print "-- "+y[5:],
	print ctext(['red','blink'],"There are errors.")
      cstderr.close()
      cstdout.close()
    print "Done."
  


elif "make" in myaction:
  print "MAKE action recognized but not implemented yet."	  
elif "test" in myaction:
  print "TEST action recognized but not implemented yet."	    