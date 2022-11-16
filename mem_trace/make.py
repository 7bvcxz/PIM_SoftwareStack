import os

os.system("g++ -static -I /home/kkm0411/gem5-22.0.0/include -o mem_test mem_trace.cpp libm5.a")
os.system("cp mem_test ..")
