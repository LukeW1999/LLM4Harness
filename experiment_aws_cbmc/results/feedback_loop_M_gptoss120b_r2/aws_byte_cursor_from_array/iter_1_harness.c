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
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t *buf = NULL;
    if (len > 0) {
        buf = malloc(len);
        __CPROVER_assume(buf != NULL);
    }
    const void *bytes = buf;

    /* 2. Save old state */
    const void *old_bytes = bytes;
    size_t old_len = len;

    /* 3. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Postconditions */
    assert(cur.ptr == (uint8_t *)old_bytes);
    assert(cur.len == old_len);

    /* 5. Unchanged inputs */
    assert(bytes == old_bytes);
    assert(len == old_len);

    /* 6. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Clean up */
    free(buf);
}
