#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* 1. Allocate a nondeterministic aws_string */
    size_t len;
    __CPROVER_assume(len <= 256);               /* bound the length */
    struct aws_string *str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
    __CPROVER_assume(str != NULL);

    /* 2. Initialize fields */
    str->allocator = aws_default_allocator();
    str->len = len;
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    /* 3. Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 4. Save old state */
    struct aws_string old = *str;   /* copies allocator and len */

    /* 5. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 6. Postconditions */
    /* Return value points to the string's byte array */
    assert(result == str->bytes);

    /* Unchanged fields */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* Validity invariant */
    assert(aws_string_is_valid(str));

    /* Clean up */
    free(str);
}
