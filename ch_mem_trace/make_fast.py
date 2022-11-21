import os

os.system("g++ -static -I /home/kkm0411/gem5-22.0.0/include -o fast_mem_test fast_mem_trace.cpp libm5.a")
os.system("cp fast_mem_test ..")
