import os

name = str(input())

os.system("g++ -pthread -static -I /home/kkm0411/gem5-22.0.0/include -o ../"+name+" "+name+".cpp libm5.a")
#os.system("mv "+name+" ..")
