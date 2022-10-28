
import os

prod = open("TOTAL_P_Nim.txt", 'r')
con = open("TOTAL_C_Nim.txt", 'r')

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
        #print(str(req))
        print(str(req)+"   "+str(p_arr[0])+"   "+str(resp)+"  "+str(total)) #'''str(c_arr[1])+"|"
