import os

os.system("g++ -static -I /home/kkm0411/gem5-22.0.0/include -o quick_test quick_test.cpp libm5.a")
os.system("cp quick_test ..")
