#include <aws/common/hash_table.h>
#include <assert.h>
#include <stdbool.h>

void aws_ptr_eq_harness() {
    const void *a;
    const void *b;
    bool result = aws_ptr_eq(a, b);
    if (a == b) {
        assert(result == true);
    } else {
        assert(result == false);
    }
}
