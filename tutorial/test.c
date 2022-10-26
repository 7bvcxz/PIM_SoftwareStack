#include <stdio.h>
#include <stdlib.h>
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define ABS(x) ((x < 0) ? (-x) : (x))

void test_add_blas()
{
	int n = 65536; // 1024 x 1024 x 32 → Tested OK!
	uint8_t *in0 = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *in1 = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *out = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *ans = (uint8_t *)malloc(sizeof(uint16_t) * n);

	for (int i = 0; i < n; i++)
	{
		((uint16_t *)in0)[i] = rand();
		((uint16_t *)in1)[i] = rand();
		((uint16_t *)ans)[i] = ((uint16_t *)in0)[i] + ((uint16_t *)in1)[i];
	}

	C_pimblasAddPreprocess(n, &in0, &in1);

	C_pim_add(n, in0, in1, out);

	int error = 0;
	for (int i = 0; i < n; i++)
		error = error + ABS(((uint16_t *)out)[i] - ((uint16_t *)ans)[i]);
	
	if (DebugMode())
		printf("ERROR: %d\n", error);

	return;
}


void test_mul_blas()
{
	int n = 65536;
	uint8_t *in0 = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *in1 = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *out = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *ans = (uint8_t *)malloc(sizeof(uint16_t) * n);

	for (int i = 0; i < n; i++)
	{
		((uint16_t *)in0)[i] = rand();
		((uint16_t *)in1)[i] = rand();
		((uint16_t *)ans)[i] = ((uint16_t *)in0)[i] * ((uint16_t *)in1)[i];
	}

	C_pimblasMulPreprocess(n, &in0, &in1);

	C_pim_mul(n, in0, in1, out);

	int error = 0;
	for (int i = 0; i < n; i++)
		error = error + ABS(((uint16_t *)out)[i] - ((uint16_t *)ans)[i]);
	
	if (DebugMode())
		printf("ERROR: %d\n", error);
	
	return;
}

void test_gemv_blas()
{
	int m = 16;
	int n = 4096;
	uint8_t *in = (uint8_t *)malloc(sizeof(uint16_t) * m);
	uint8_t *w = (uint8_t *)malloc(sizeof(uint16_t) * m * n);
	uint8_t *out = (uint8_t *)malloc(sizeof(uint16_t) * n);
	uint8_t *ans = (uint8_t *)malloc(sizeof(uint16_t) * n);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			((uint16_t *)w)[i * m + j] = rand();

	for (int j = 0; j < m; j++)
		((uint16_t *)in)[j] = rand();

	for (int i = 0; i < n; i++)
		for (int j = 0; j < m; j++)
			((uint16_t *)ans)[i] = ((uint16_t *)ans)[i] + ((uint16_t *)w)[i * m + j] * ((uint16_t *)in)[j];

	C_pimblasGemvPreprocess(m, n, &w);

	C_pim_gemv(m, n, in, w, out);

	int error = 0;
	for (int i = 0; i < n; i++)
		error = error + ABS(((uint16_t *)out)[i] - ((uint16_t *)ans)[i]);
	
	if (DebugMode())
		printf("ERROR: %d\n", error);
	return;
}

int main(int argc, char **argv)
{
	blas_init(1);

	// test_add_blas();
	// test_mul_blas();
	test_gemv_blas();
	
	return 0;
}
