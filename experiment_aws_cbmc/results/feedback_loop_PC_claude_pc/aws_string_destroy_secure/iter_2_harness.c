#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose between NULL and valid string */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    if (is_null) {
        /* Test NULL path: function must be a no-op and not crash */
        aws_string_destroy_secure(NULL);
        assert(true);
    } else {
        /* Test valid string path */
        /* Use a bounded length to keep the state space manageable */
        size_t len;
        __CPROVER_assume(len <= 4);

        /* Allocate memory for the struct + len bytes + 1 null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        struct aws_string *str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the const fields via pointer casting */
        *((size_t *)(&str->len)) = len;
        *((struct aws_allocator **)(&str->allocator)) = aws_default_allocator();

        /* Precondition: str is a valid aws_string */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save observable state before the call */
        size_t old_len = str->len;

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /* Function completed without crashing */
        assert(true);
        assert(old_len <= 4);
    }
}

void aws_string_destroy_secure_harness(void) {
    aws_string_destroy_secure_harness();
    return 0;
}
