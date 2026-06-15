#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "aws/common/common.h"

void aws_ptr_eq_harness() {
    const void *a;
    const void *b;
    bool result = aws_ptr_eq(a, b);
    assert(result == (a == b));
}
