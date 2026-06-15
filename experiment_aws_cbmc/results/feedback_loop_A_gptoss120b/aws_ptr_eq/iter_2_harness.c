#include <assert.h>
#include <aws/common/hash_table.h>
#include "proof_helpers/make_common_data_structures.h"

extern uint64_t nondet_uint64_t(void);

void aws_ptr_eq_harness(void) {
    const void *a = (const void *)nondet_uint64_t();
    const void *b = (const void *)nondet_uint64_t();

    bool result = aws_ptr_eq(a, b);

    assert(result == (a == b));
}
