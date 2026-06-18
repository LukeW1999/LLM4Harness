#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    struct aws_string *str = nondet_allocate_string();
    if (str) {
        __CPROVER_assume(aws_string_is_valid(str));
        // Ensure the allocator is valid, e.g., by using the default allocator
        // This avoids problems with NULL allocator or missing function pointers
        struct aws_allocator *alloc = aws_default_allocator();
        memcpy((void *)&str->allocator, &alloc, sizeof(alloc));
    }
    aws_string_destroy(str);
}
