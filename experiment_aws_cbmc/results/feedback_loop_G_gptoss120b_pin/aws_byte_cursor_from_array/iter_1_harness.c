#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare nondeterministic inputs */
    const void *bytes;
    size_t len;

    /* nondeterministically decide whether the pointer is NULL or points to allocated memory */
    if (nondet_bool()) {
        bytes = NULL;
    } else {
        size_t alloc_size = nondet_size_t();
        __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
        bytes = malloc(alloc_size);
        __CPROVER_assume(bytes != NULL);
    }

    len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
