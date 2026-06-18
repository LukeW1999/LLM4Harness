#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Nondeterministic inputs, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        /* Ensure the allocated memory is non‑NULL for a non‑zero length */
        __CPROVER_assume(bytes != NULL);
    } else {
        /* For zero length the pointer may be NULL or any value; we choose NULL */
        bytes = NULL;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Post‑condition: fields must match the inputs */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* 4. The returned cursor must satisfy the validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
