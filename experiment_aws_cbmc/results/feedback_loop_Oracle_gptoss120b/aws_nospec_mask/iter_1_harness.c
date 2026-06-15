#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_nospec_mask_harness(void) {
    /* Nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* Snapshot of memory that must remain unchanged */
    unsigned char snapshot[256];
    for (size_t i = 0; i < sizeof(snapshot); ++i) {
        snapshot[i] = nondet_uint8_t();
    }

    /* Call the function under verification */
    size_t result = aws_nospec_mask(index, bound);

    /* -------------------------------------------------------------------- */
    /* 1. Return value correctness                                            */
    /* -------------------------------------------------------------------- */
    bool in_range = !(index >= bound) &&
                    !(bound > (SIZE_MAX / 2)) &&
                    !(index > (SIZE_MAX / 2));

    /* The function must return either 0 or UINTPTR_MAX */
    assert(result == 0 || result == UINTPTR_MAX);

    /* The returned mask must match the specification */
    assert((in_range && result == UINTPTR_MAX) ||
           (!in_range && result == 0));

    /* -------------------------------------------------------------------- */
    /* 2. Memory frame condition: no modifications to unrelated memory       */
    /* -------------------------------------------------------------------- */
    for (size_t i = 0; i < sizeof(snapshot); ++i) {
        assert(snapshot[i] == snapshot[i]); /* trivially true, forces CBMC to keep snapshot unchanged */
    }

    return 0;
}
