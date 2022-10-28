
import os
import numpy as np

pkt_file = "count_packets.txt"
comp_time_f = "TOTAL_C.txt"
exec_f = "exec_time.txt"

dirs = ["26/"] #"6/","8/"]


for d in dirs:
    pktF = d+pkt_file
    totalF = d+comp_time_f
    execF = d+exec_f
    packets=[]
    times=[]
    execs=[]
    for l in open(pktF, 'r'):
        packets = packets + [int(l.strip())]
    for l in open(totalF, 'r'):
        times=times+[int(l.strip())]
    for l in open(execF, 'r'):
        for e in l.split(", "):
            execs=execs+[int(e.strip())]


    pkt_mean = np.mean(packets)
    time_mean = np.mean(times)
    exec_mean= np.mean(execs)
    fname=d+"means.txt"
    f = open(fname, "a")
    f.write(str(pkt_mean)+":"+str(time_mean)+":"+str(exec_mean))
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
