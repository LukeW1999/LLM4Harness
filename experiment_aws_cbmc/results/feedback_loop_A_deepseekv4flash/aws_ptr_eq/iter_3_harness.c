#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "aws/common/common.h"

void aws_ptr_eq_harness() {
    const void *a;
    const void *b;
    
    // Make a and b nondeterministic but ensure both branches are reachable
    int nondet_int;
    if (nondet_int) {
        a = (const void *)1;
        b = (const void *)1;
    } else {
        a = (const void *)1;
        b = (const void *)2;
    }
    
    bool result = aws_ptr_eq(a, b);
    assert(result == (a == b));
}
