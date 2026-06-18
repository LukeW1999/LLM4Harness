#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

void aws_ptr_eq_harness(void) {
    const void *a = nondet_bool() ? (const void *)(uintptr_t)nondet_uint64_t() : NULL;
    const void *b = nondet_bool() ? (const void *)(uintptr_t)nondet_uint64_t() : NULL;
    
    bool result = aws_ptr_eq(a, b);
    
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
