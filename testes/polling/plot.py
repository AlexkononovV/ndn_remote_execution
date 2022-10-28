import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import norm
import os

dirs = ["0.8/","1.6/","2.4/","3.2/","4.0/","4.8/","5.6/","6.4/","7.2/","8.0/","10/"]

x_poll=[0.8,1.6,2.4,3.2,4.0,4.8,5.6,6.4,7.2,8.0,10]
y1_time=[]
y2_pkts=[]

for d in dirs:
    fname = d+"means.txt"
    for l in open(fname, 'r'):
        d = l.strip().split(":")
        y2_pkts = y2_pkts + [float(d[0])]
        y1_time = y1_time + [float(d[1])]

print(x_poll)
print(y1_time)
print(y2_pkts)
fig, ax = plt.subplots()
#ax.plot(x_inst, y_inst, label="service instantiation time", marker='o')
#ax.plot(x_route_create, y_rc, label="service route creation time", marker='x')
#ax.plot(x_del, y_del, label="service deletion time", marker='*')
ax2 = ax.twinx()
ax.plot(x_poll, y1_time, label="relation between polling interval and completion time", color="#dd0011" )
ax2.plot(x_poll, y2_pkts, label="relation between polling interval and number of packets", color="green")
ax.set_xlabel('time ms')
ax.set_ylabel('time ms')
ax2.set_ylabel('packets')
#ax.set_title("Common Distribution Function")
ax.legend()
ax2.legend()
ax.yaxis.grid()

plt.show()
