import os

os.system("g++ -static -I /home/kkm0411/gem5-22.0.0/include -o ch_mem_test ch_mem_trace.cpp libm5.a")
os.system("cp ch_mem_test ..")
