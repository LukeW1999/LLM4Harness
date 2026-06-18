#include <aws/common/hash_table.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

void aws_ptr_eq_harness() {
    void *ptr_a = nondet_bool() ? malloc(1) : NULL;
    void *ptr_b = nondet_bool() ? malloc(1) : NULL;
    
    const void *a = ptr_a;
    const void *b = ptr_b;
    
    bool result = aws_ptr_eq(a, b);
    
    assert(result == (a == b));
    
    if (ptr_a) free(ptr_a);
    if (ptr_b) free(ptr_b);
}
