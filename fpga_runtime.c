#include "fpga_runtime.h"

uint64_t pimExecution(uint32_t addr, uint32_t *data, int iswrite) {
	printf("   fpga: %x\n", addr);
	return 1;
}


