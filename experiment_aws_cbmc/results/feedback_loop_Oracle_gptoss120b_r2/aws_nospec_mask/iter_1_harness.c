#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic value generators for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_nospec_mask_harness(void) {
    /* Nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Frame condition: allocate a memory region and fill with nondet data */
    const size_t MEM_SIZE = 64;
    uint8_t *mem = malloc(MEM_SIZE);
    __CPROVER_assume(mem != NULL);
    for (size_t i = 0; i < MEM_SIZE; ++i) {
        mem[i] = nondet_uint8_t();
    }

    /* Make a copy of the original memory contents */
    uint8_t *mem_copy = malloc(MEM_SIZE);
    __CPROVER_assume(mem_copy != NULL);
    for (size_t i = 0; i < MEM_SIZE; ++i) {
        mem_copy[i] = mem[i];
    }

    /* Call the function under test */
    size_t result = aws_nospec_mask(index, bound);

    /* Postcondition 1: result must be either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);

    /* Expected behavior according to the specification */
    bool in_range = !(index >= bound ||
                      bound > (SIZE_MAX / 2) ||
                      index > (SIZE_MAX / 2));

    if (in_range) {
        assert(result == UINTPTR_MAX);
    } else {
        assert(result == 0);
    }

    /* Postcondition 3: memory frame condition – no modifications */
    for (size_t i = 0; i < MEM_SIZE; ++i) {
        assert(mem[i] == mem_copy[i]);
    }

    free(mem);
    free(mem_copy);
    return 0;
}
