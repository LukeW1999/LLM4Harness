#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Nondeterministic inputs, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    const void *bytes;
    if (nondet_bool()) {
        /* NULL pointer case */
        bytes = NULL;
    } else {
        /* Allocate a readable buffer of the requested length */
        uint8_t *tmp = malloc(len);
        __CPROVER_assume(tmp != NULL || len == 0);
        bytes = tmp;
    }

    /* Ensure the pointer/length pair satisfies the cursor validity predicate */
    __CPROVER_assume(!(len > 0) || bytes != NULL);

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Post‑condition asserts */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 4. Invariant: the returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 5. Clean up any allocated memory */
    if (bytes != NULL) {
        free((void *)bytes);
    }
}
