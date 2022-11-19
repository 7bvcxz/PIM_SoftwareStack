#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <chrono>
#include <gem5/m5ops.h>
#include "m5_mmap.h"

#define LEN_PIM 0x100000000

int fd;
std::ifstream fm;
std::string line;
uint8_t* pim_mem;
uint64_t pim_base;

int num_line;
bool *file_is_write;
uint32_t *file_hex_addr;

uint32_t burstSize = 32;

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

void set_trace_file(char **argv, char option) {
	std::cout << " > set trace file\n";
	fm.open("./ch_mem_trace/pim_"+std::string(argv[1])+option+".txt");
	
	while(std::getline(fm, line))
		num_line++;
	fm.close();

	file_is_write = (bool*)calloc(num_line, sizeof(bool));
	file_hex_addr = (uint32_t*)calloc(num_line, sizeof(uint32_t));
	
	int i = 0;
	fm.open("./ch_mem_trace/pim_"+std::string(argv[1])+option+".txt");
	while(std::getline(fm, line)) {
		std::stringstream linestream(line);
		int is_write;
		uint32_t hex_addr;

		linestream >> is_write >> hex_addr;
	
		if (is_write == 0) {
			file_is_write[i] = false;
		} else {
			file_is_write[i] = true;
		}
		file_hex_addr[i] = hex_addr;
		i++;
	}
}

void set_pim_device() {
	std::cout << " > set pim device\n";
	fd = open("/dev/PIM", O_RDWR|O_SYNC);
	
	if (fd < 0)
		std::cout << "Open /dev/PIM failed...\n";
	else
		std::cout << "Opened /dev/PIM !\n";

	pim_mem = (uint8_t*)mmap(NULL, LEN_PIM, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	pim_base = (uint64_t)pim_mem;
	uint8_t* buffer1 = (uint8_t*)calloc(burstSize*16, sizeof(uint8_t));
	
	for (int i=0; i<num_line; i++)
		std::memcpy(pim_mem + file_hex_addr[i], buffer1, burstSize*16);

	m5op_addr = 0xFFFF0000;
	map_m5_mem();
}

void set_normal_device() {
	std::cout << " > set normal device\n";
	
	pim_mem = (uint8_t*)calloc(LEN_PIM, sizeof(uint8_t));
	pim_base = (uint64_t)pim_mem;
}

void send() {
	std::cout << " > trace and send\n";

	uint8_t* buffer2 = (uint8_t*)calloc(burstSize*16, sizeof(uint8_t));
	auto start = Time::now();
	//system("sudo m5 dumpstats");
	//m5_work_begin_addr(0,0);
	//m5_dump_stats(0,0);
	m5_reset_stats(0,0);

	for (int i=0; i<num_line; i++)
		std::memcpy(buffer2, pim_mem + file_hex_addr[i], burstSize*16);

	//system("sudo m5 dumpstats");
	//m5_work_end_addr(0,0);
	m5_dump_stats(0,0);
	auto end = Time::now();
	
	std::cout << "All trace ended\n";
	fsec time = end - start;
	std::cout << time.count() << "s\n";
}

int main(int argc, char **argv) {
	char option;
	std::cout << "option : 1 / 2 / 3\nenter option : ";
	std::cin >> option;

	if (argc <= 1) {
		std::cout << "add, mul, mac, bn, gemv, lstm\n";
		return -1;
	}

	set_trace_file(argv, option);

	set_pim_device();
	//set_normal_device();
	
	system("sudo m5 checkpoint");
    system("echo CPU Switched!");

	send();

	return 0;
}
