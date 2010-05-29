from arp_output import ctext


def shorthelp():
	print
	print ctext(['bold'],"Usage:")
	print "   "+ctext(['turquoise'],"arp")+" [ "+ctext(['green'],"action")+" ] [ "+ctext(['turquoise'],"target")+" ] [ ... ]"
	print
	print ctext(['bold'],"Actions:")+" [ "+ctext(['green'],"--pack")+" | "+ctext(['green'],"--unpack")+" | "+ctext(['green'],"--list")+" ]" 
#  	"+ctext(['green'],"--delete")+" | "+ctext(['green'],"--make")+" | 
# 	"+ctext(['green'],"--test")+" ]"
	print

