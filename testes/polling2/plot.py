import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import norm
import os

dirs = ["2/","4/","6/","8/","10/","12/","14/","16/","18/","20/","23/","26/"]

x_poll=[2,4,5,6,11,12,14,16,18,20,23,26]
y1_time=[]
y2_pkts=[]
y_exec=[]

for d in dirs:
    fname = d+"means.txt"
    for l in open(fname, 'r'):
        d = l.strip().split(":")
        y2_pkts = y2_pkts + [float(d[0])]
        y1_time = y1_time + [float(d[1])]
        y_exec = y_exec + [float(d[2])]

print(x_poll)
print(y1_time)
print(y2_pkts)
print(y_exec)
fig, ax = plt.subplots()
#ax.plot(x_inst, y_inst, label="service instantiation time", marker='o')
#ax.plot(x_route_create, y_rc, label="service route creation time", marker='x')
#ax.plot(x_del, y_del, label="service deletion time", marker='*')
ax2 = ax.twinx()
ax.plot(x_poll, y1_time, label="relation between polling interval and completion time", color="#dd0011" )
ax2.plot(x_poll, y2_pkts, color="green")
ax.plot(x_poll, y_exec, color="blue", label="execution time" )

ax.set_xlabel('time ms')
ax.set_ylabel('time ms')
ax2.set_ylabel('packets')

#ax.axhline(y = 19.2, color = 'r', linestyle = '-', label = "execution time")

#ax.set_title("Common Distribution Function")
ax.legend(loc=(0,1.06))
ax2.legend(["relation between polling interval and number of packets"],loc=(0, 1.01))
ax.yaxis.grid()

plt.show()
