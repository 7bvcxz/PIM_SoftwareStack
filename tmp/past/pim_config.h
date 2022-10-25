#ifndef __PIM_CONFIG_H_
#define __PIM_CONFIG_H_

#include <iostream>
#include <fcntl.h>		  // O_RDWR, O_SYNC
#include <sys/mman.h>	  // MAP_SHARED, PROT_READ
#include <random>		  // random_device
#include "half.hpp"

typedef uint16_t			unit_t;

// SIZE IS BYTE
#define UNIT_SIZE			(int)(sizeof(unit_t))
#define WORD_SIZE			32
#define UNITS_PER_WORD		(WORD_SIZE / UNIT_SIZE)
#define GRF_SIZE			(8 * WORD_SIZE)
#define SRF_SIZE			(8 * UNIT_SIZE)

#define LEN_PIM				0x100000000
#define BASE_PIM_ADDR		0x140000000

#define EVEN_BANK 0
#define ODD_BANK 1

#define NUM_WORD_PER_ROW	  32
#define NUM_UNIT_PER_WORD	  16
#define	NUM_CHANNEL			  16
#define NUM_BANK_PER_CHANNEL  16
#define NUM_BANK			  (NUM_BANK_PER_CHANNEL * NUM_CHANNEL)
#define SIZE_WORD			  32
#define SIZE_ROW			  (SIZE_WORD * NUM_WORD_PER_ROW)

#define MAP_SBMR             0x3fff
#define MAP_ABMR             0x3ffe
#define MAP_PIM_OP_MODE      0x3ffd
#define MAP_CRF              0x3ffc
#define MAP_GRF              0x3ffb
#define MAP_SRF              0x3ffa

struct Address {
    Address()
        : channel(-1), rank(-1), bankgroup(-1), bank(-1), row(-1), column(-1) {}
    Address(int channel, int rank, int bankgroup, int bank, int row, int column)
        : channel(channel),
          rank(rank),
          bankgroup(bankgroup),
          bank(bank),
          row(row),
          column(column) {}
    Address(const Address& addr)
        : channel(addr.channel),
          rank(addr.rank),
          bankgroup(addr.bankgroup),
          bank(addr.bank),
          row(addr.row),
          column(addr.column) {}
    int channel;
    int rank;
    int bankgroup;
    int bank;
    int row;
    int column;
};

enum class PIM_OP {
    ADD = 0,
    MUL,
    BN,
    GEMV,
    LSTM,
    RELU
};

enum class PIM_REG {
	SRF = 0,
	GRF,
	CRF,
	ABMR,
	SBMR,
	PIM_OP_MODE,
};

enum class PIM_CMD {
	WRITE_SRF_INPUT,
	WRITE_GRF_INPUT,
	SB_MODE,
	AB_MODE,
	PIM_OP_MODE,
	READ_WEIGHT_1COL,
	READ_WEIGHT_8COL,
	READ_INPUT_1COL,
	READ_INPUT_8COL,
	READ_OUTPUT_1COL,
	READ_OUTPUT_8COL,
	WRITE_WEIGHT_1COL,
	WRITE_WEIGHT_8COL,
	WRITE_INPUT_1COL,
	WRITE_INPUT_8COL,
	WRITE_OUTPUT_1COL,
	WRITE_OUTPUT_8COL,
	NULL_READ,	
};

class PIM_OP_ATTRS {
 public:
    PIM_OP_ATTRS(){};
	~PIM_OP_ATTRS(){};
	
	void ADD(uint8_t *x, uint8_t *y, uint8_t *z, int len);
	void MUL(uint8_t *x, uint8_t *y, uint8_t *z, int len);
	void BN(uint8_t *x, uint8_t *y, uint8_t *z, int len);
	void GEMV(uint8_t *y, uint8_t *z, int len_in, int len_out);
	void LSTM(uint8_t *x, uint8_t *y, uint8_t *z, int len);

	int len_in;
	int len_out;
	bool input_is_pim;
	PIM_OP pim_op;

	int code_iter;
	int code0_iter;
	int code1_iter;
};

class PIMKernel {
 public:
	PIM_OP pim_op;
	uint32_t code0[32];
    uint32_t code1[32];
	PIM_CMD* code0_cmd;
	PIM_CMD* code1_cmd;
	int code0_num_cmds;
	int code1_num_cmds;

	void SetMicrokernelCode(PIM_OP op) {
		if (op == (PIM_OP::ADD)) {
			code0[0]  = 0b01000010000000001000000000000000; // MOV(A)  GRF_A[A0]  BANK
			code0[1]  = 0b00010000000001000000100000000111; // JUMP    -1         7
			code0[2]  = 0b10000010000010001000000000000000; // ADD(A)  GRF_A[A0]  BANK      GRF_A[A0]
			code0[3]  = 0b00010000000001000000100000000111; // JUMP    -1         7
			code0[4]  = 0b01000000010000001000000000000000; // MOV(A)  BANK       GRF_A[A0]
			code0[5]  = 0b00010000000001000000100000000111; // JUMP    -1         7
			code0[6]  = 0b01000010000000001000000000000000; // MOV(A)  GRF_A[A0]  BANK
			code0[7]  = 0b00010000000001000000100000000111; // JUMP    -1         7
			code0[8]  = 0b10000010000010001000000000000000; // ADD(A)  GRF_A[A0]  BANK      GRF_A[A0]
			code0[9]  = 0b00010000000001000000100000000111; // JUMP    -1         7
			code0[10] = 0b01000000010000001000000000000000; // MOV(A)  BANK       GRF_A[A0]
			code0[11] = 0b00010000000001000000100000000111; // JUMP    -1         7
			code0[12] = 0b00100000000000000000000000000000; // EXIT
			pim_op = PIM_OP::ADD;
		}
		else if (op == (PIM_OP::MUL)) {
			code0[0]  = 0b01000010000000001000000000000000; // MOV(A)  GRF_A[A0]  BANK
            code0[1]  = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[2]  = 0b10010010000010001000000000000000; // MUL(A)  GRF_A[A0]  BANK      GRF_A[A0]
            code0[3]  = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[4]  = 0b01000000010000001000000000000000; // MOV(A)  BANK       GRF_A[A0]
            code0[5]  = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[6]  = 0b01000010000000001000000000000000; // MOV(A)  GRF_A[A0]  BANK
            code0[7]  = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[8]  = 0b10010010000010001000000000000000; // MUL(A)  GRF_A[A0]  BANK      GRF_A[A0]
            code0[9]  = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[10] = 0b01000000010000001000000000000000; // MOV(A)  BANK       GRF_A[A0]
            code0[11] = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[12] = 0b00100000000000000000000000000000; // EXIT
			pim_op = PIM_OP::MUL;
		}
		else if (op == (PIM_OP::BN)) {
			code0[0]  = 0b01000010000000001000000000000000;  // MOV(A)  GRF_A[A0] BANK
			code0[1]  = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[2]  = 0b10010010000010001000000000000000;  // MUL(A)  GRF_A[A0] BANK      GRF_A[A0]
			code0[3]  = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[4]  = 0b10000010000010001000000000000000;  // ADD(A)  GRF_A[A0] BANK      GRF_A[A0]
			code0[5]  = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[6]  = 0b01000000010000001000000000000000;  // MOV(A)  BANK      GRF_A[A0]
			code0[7]  = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[8]  = 0b01000010000000001000000000000000;  // MOV(A)  GRF_A[A0] BANK
			code0[9]  = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[10] = 0b10010010000010001000000000000000;  // MUL(A)  GRF_A[A0] BANK      GRF_A[A0]
			code0[11] = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[12] = 0b10000010000010001000000000000000;  // ADD(A)  GRF_A[A0] BANK      GRF_A[A0]
			code0[13] = 0b00010000000001000000100000000111;  // JUMP    -1        7 
			code0[14] = 0b01000000010000001000000000000000;  // MOV(A)  BANK      GRF_A[A0]
			code0[15] = 0b00010000000001000000100000000111;  // JUMP    -1        7
			code0[16] = 0b00100000000000000000000000000000;  // EXIT
			pim_op = PIM_OP::BN;
		}
		else if (op == (PIM_OP::GEMV)) {
	        code0[0] = 0b10100100001000001000100000000000; // MAC(A)  GRF_B[A0]  BANK      SRF_M[A0]
            code0[1] = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[2] = 0b00100000000000000000000000000000; // EXIT

            code1[0] = 0b10100100001000001000100000000000; // MAC(A)  GRF_B[A0]  BANK      SRF_M[A0]
            code1[1] = 0b00010000000001000000100000000111; // JUMP    -1         7
            code1[2] = 0b01000000100000000000000000000000; // MOV     BANK       GRF_B[0]
            code1[3] = 0b00100000000000000000000000000001; // EXIT

			code0_num_cmds = 1;
			code0_cmd = (PIM_CMD*)malloc(sizeof(PIM_CMD)*code0_num_cmds);
			code0_cmd[0] = PIM_CMD::READ_WEIGHT_8COL;
			
			code1_num_cmds = 2;
			code1_cmd = (PIM_CMD*)malloc(sizeof(PIM_CMD)*code1_num_cmds);
			code1_cmd[0] = PIM_CMD::READ_WEIGHT_8COL;
			code1_cmd[1] = PIM_CMD::WRITE_OUTPUT_1COL;

			pim_op = PIM_OP::GEMV;
		}
		else if (op == (PIM_OP::LSTM)) {
            code0[0] = 0b10100100001000001000100000000000; // MAC(A)  GRF_B[0]  BANK  SRF_M[A0]
            code0[1] = 0b00010000000001000000100000000111; // JUMP    -1         7
            code0[2] = 0b00100000000000000000000000000000; // EXIT

    		code1[0] = 0b10000010100000000000100010000000; // ADD     GRF_A[0]  GRF_B[0]  BANK
    		code1[1] = 0b10000100010000000000100110000000; // ADD     GRF_B[1]  GRF_A[0]  BANK
    		code1[2] = 0b01000000100000000000000000010000; // MOV     BANK      GRF_B[1]
    		code1[3] = 0b00100000000000000000000000000000; // EXIT
			pim_op = PIM_OP::LSTM;
		}
	}	
};

#endif  // __PIM_CONFIG_H_
