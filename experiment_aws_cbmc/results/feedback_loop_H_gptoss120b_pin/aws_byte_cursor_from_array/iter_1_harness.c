#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Nondeterministic inputs */
    const void *bytes;
    size_t len = nondet_size_t();

    /* Bound the length to keep the state space manageable */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a readable buffer when length is non‑zero */
    if (len > 0) {
        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        bytes = buf;
    } else {
        /* When length is zero the pointer may be NULL or any value; we choose NULL */
        bytes = NULL;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Post‑conditions */
    /* The returned cursor must reference the same memory and length */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* The cursor must satisfy its validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
