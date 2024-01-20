#!/usr/bin/env python3
from os import sys
import scapy.all as scapy

p = scapy.rdpcap(sys.argv[1])

#print(p[0])
#print(len(p[0]))
#raw_pkt = scapy.raw(p[0])
#payload = raw_pkt[42:]
#print(payload)
#print(len(payload))
f = open(sys.argv[2],"wb")
for pkt in  p:
    raw_pkt = scapy.raw(pkt)
    f.write(raw_pkt[42:])
f.close()


