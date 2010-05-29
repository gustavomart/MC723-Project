# Some few functions for color text output
# Based on gentoo emerge code

esc_seq = "\x1b["

codes={}
codes["reset"]     = esc_seq + "39;49;00m"

codes["bold"]      = esc_seq + "01m"
codes["faint"]     = esc_seq + "02m"
codes["standout"]  = esc_seq + "03m"
codes["underline"] = esc_seq + "04m"
codes["blink"]     = esc_seq + "05m"
codes["overline"]  = esc_seq + "06m"  # Who made this up? Seriously.

codes["teal"]      = esc_seq + "36m"
codes["turquoise"] = esc_seq + "36;01m"

codes["fuchsia"]   = esc_seq + "35;01m"
codes["purple"]    = esc_seq + "35m"

codes["blue"]      = esc_seq + "34;01m"
codes["darkblue"]  = esc_seq + "34m"

codes["green"]     = esc_seq + "32;01m"
codes["darkgreen"] = esc_seq + "32m"

codes["yellow"]    = esc_seq + "33;01m"
codes["brown"]     = esc_seq + "33m"

codes["red"]       = esc_seq + "31;01m"
codes["darkred"]   = esc_seq + "31m"


def ctext(color,text):
  out=[]
  for x in color:
    out.append(codes[x])
  return ''.join(out)+text+codes["reset"]