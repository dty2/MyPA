#include <stdint.h>
#include <stdio.h>

#define NREG 4
#define NMEM 16

/*
bit field
struct{
	int a : 1;
	int b : 2;
	int c : 3;
}
a only occupy 1 bit, b occupy 2 bit and c occupy 3 bit
*/

typedef union {
	struct { uint8_t rs : 2, rt : 2, op : 4;  } rtype;
	struct { uint8_t addr : 4      , op : 4;  } mtype;
	uint8_t inst;
} inst_t;

#define DECODE_R(inst) uint8_t rt = (inst).rtype.rt, rs = (inst).rtype.rs
#define DECODE_M(inst) uint8_t addr = (inst).mtype.addr

uint8_t pc = 0;
// pc , because c haven't got 4 bit type , so we use 8 bit type to express it.

uint8_t R[NREG] = {}; // register

uint8_t M[NMEM] = {
	//0000 mov rt,rs R(rt) <- R(rs)
	//0001 add rt,rs R(rt) <- R(rs) + R(rt)
	//1110 addr      R(rt) <- R(rs)
	//1111 addr      R(rt) <- R(rs)

	// 0b means the 11100110 is binary number
	0b11100110, // 0b 1110 0110 load 6#
	0b00000100, // 0b 0000 0100 mov r1, r0
	0b11100101, // 0b 1110 0101 load 5#
	0b00010001, // 0b 0001 0001 add r0,r1
	0b11110111, // 0b 1111 0111 store 7#
	0b00010000, // 0b 0001 0000 x = 16
	0b00100001, // 0b 0010 0001 y = 33
	0b00000000, // 0b 0000 0000 z = 0
};
// memory, include the program about calculate expression to calculate  z = x + y

int halt = 0; // end flag 

//execute one command
void exec_once() {
	inst_t this;
	this.inst = M[pc]; // get command
	switch (this.rtype.op) {
		// op decode    op_num decode       execute
		case 0b0000: { DECODE_R(this); R[rt]   = R[rs];   break;  }
		case 0b0001: { DECODE_R(this); R[rt]  += R[rs];   break;  }
		case 0b1110: { DECODE_M(this); R[0]    = M[addr]; break;  }
		case 0b1111: { DECODE_M(this); M[addr] = R[0];    break;  }
		default:
			printf("Invalid instruction with opcode = %x, halting...\n", this.rtype.op);
			halt = 1;
			break;
	}
	pc ++;
}

int main() {
	while (1) {
		exec_once();
		if (halt) break;
	}
	printf("The result of 16 + 33 is %d\n", M[7]);
	return 0;
		
}
//my note
execute -> exec_once -> isa_exec_once
