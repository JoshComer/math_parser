#ifndef IOF_NUM_HEADER_FILE
#define IOF_NUM_HEADER_FILE

#include <stdio.h>
#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

#include "jc_util.h"


// int or float
typedef union _iof_union {
	mpz_t integer;
	mpfr_t floating;
} _iof_union;

typedef enum iof_type {
	IOF_TYPE_INTEGER,
	IOF_TYPE_FLOATING
} iof_type;

typedef struct iof_num {
	_iof_union num;
	iof_type type;
	bool inited;
} iof_num;

// object management
void iof_init_int(iof_num * to_init);
void iof_reinit_int(iof_num * to_reinit);
void iof_clear(iof_num * to_clear);

bool _iof_convert_to_float(iof_num * integer);

// arithmetic
bool iof_add(iof_num * operand1, iof_num * operand2);
bool iof_subtract(iof_num * operand1, iof_num * operand2);
bool iof_multiply(iof_num * operand1, iof_num * operand2);
bool iof_divide(iof_num * operand1, iof_num * operand2);
bool iof_modulus(iof_num * operand1, iof_num * operand2);
bool iof_exponentiation(iof_num * operand1, iof_num * operand2);

// comparison
int iof_cmp_si(iof_num * operand1, int operand2);
int iof_cmp_d(iof_num * operand1, double operand2);
bool iof_set_from_str(iof_num * result, char * string);

// IO
void iof_out_str(iof_num * num);

#endif