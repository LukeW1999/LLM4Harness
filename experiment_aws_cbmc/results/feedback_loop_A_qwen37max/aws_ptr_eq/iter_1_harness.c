#include <aws/common/hash_table.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

void aws_ptr_eq_harness() {
    int x, y;
    const void *a = &x;
    const void *b = &y;
    
    if (nondet_bool()) { a = NULL; }
    if (nondet_bool()) { b = NULL; }
    if (nondet_bool()) { b = a; }
    
    bool result = aws_ptr_eq(a, b);
    
    assert(result == (a == b));
}
