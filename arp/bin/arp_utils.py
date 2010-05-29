import re,sys
from arp_output import ctext

def getdefstructure(filename):
  platstruct=[]
  reip = re.compile(r"IP *:= *")
  reis = re.compile(r"IS *:= *")
  repc = re.compile(r"PROCESSOR *:= *")
  resw = re.compile(r"SW *:= *")
  rewr = re.compile(r"WRAPPER *:= *")
  for lines in open(filename):
    mip = reip.match(lines)
    mis = reis.match(lines)
    mpc = repc.match(lines)
    msw = resw.match(lines)
    mwr = rewr.match(lines)
    if mip: 
      for y in lines[mip.end():].split(): platstruct.append("ip/"+y)
    elif mis: 
      for y in lines[mis.end():].split(): platstruct.append("is/"+y)
    elif msw: 
      for y in lines[msw.end():].split(): platstruct.append("sw/"+y)
    elif mwr: 
      for y in lines[mwr.end():].split(): platstruct.append("wrappers/"+y)
    elif mpc: 
      for y in lines[mpc.end():].split(): platstruct.append("processors/"+y)
    platname = "reserved"
  try:
    platname
  except:
    print ctext(['red'],"!!! Error: platform name not specified.")
    sys.exit(1)
  return platname, platstruct
