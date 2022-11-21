import os

name = str(input())

out_f = open("pim_"+name+"_t1.txt", "w")
in_f = open(name+"_t1.txt", "r")
flag = 0

lines = in_f.readlines()
for line in lines:
    items = line.split()
    if (items[0] == '3'):
        break

    if (flag == 1):
        out_f.write(line)
   
    if (items[0] == '2'):
        flag = flag + 1;


out_f.close()
in_f.close()
