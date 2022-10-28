
import os
import numpy as np

pkt_file = "count_packets.txt"
comp_time_f = "TOTAL_C.txt"

dirs = ["0.8/","1.6/","2.4/","3.2/","4.0/","4.8/","5.6/","6.4/","7.2/","8.0/","10/"]


for d in dirs:
    pktF = d+pkt_file
    totalF = d+comp_time_f
    packets=[]
    times=[]
    for l in open(pktF, 'r'):
        packets = packets + [int(l.strip())]
    for l in open(totalF, 'r'):
        times=times+[int(l.strip())]

    pkt_mean = np.mean(packets)
    time_mean = np.mean(times)
    fname=d+"means.txt"
    f = open(fname, "a")
    f.write(str(pkt_mean)+":"+str(time_mean))
    f.close()

'''
print(" req   exec   resp   total/sum")
for line in con: # read file 1 one line after the other
        line2 = prod.readline() # read a line of file 2
        c_arr=line.strip().split(".")[0].split(",")
        p_arr=line2.strip().split(".")[0].split(", ")
        #print( c_arr[0], c_arr[1])
        #print( p_arr[0], p_arr[1])
        req = int(c_arr[0]) - int(p_arr[0])
        resp = max((int(c_arr[1])-int(c_arr[0])),int(p_arr[1]))
        total = int(req)+int(p_arr[0])+int(resp)
        print(str(total))
        #print(str(req)+"   "+str(p_arr[0])+"   "+str(resp)+"  "+str(c_arr[1])+"|"+str(total))

'''
