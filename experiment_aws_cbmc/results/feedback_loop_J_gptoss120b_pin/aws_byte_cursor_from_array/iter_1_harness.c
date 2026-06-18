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
    /* 1. Declare nondeterministic inputs and bound them */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    const void *bytes;
    if (len > 0) {
        uint8_t *tmp = malloc(len);
        __CPROVER_assume(tmp != NULL);
        bytes = tmp;
    } else {
        /* For zero length the pointer may be NULL or any value; using NULL is safe */
        bytes = NULL;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions */
    assert(cur.len == len);
    assert(cur.ptr == (uint8_t *)bytes);

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
