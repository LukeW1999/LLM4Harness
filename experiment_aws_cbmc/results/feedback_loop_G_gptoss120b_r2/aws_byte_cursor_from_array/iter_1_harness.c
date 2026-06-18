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
    /* 1. Non‑deterministic length, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a buffer for the input array (if any) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = (uint8_t *)malloc(len);
        /* Ensure the allocated memory is readable – malloc already provides that */
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array((const void *)bytes, len);

    /* 4. Post‑conditions */
    /* The returned cursor must reference the supplied buffer and length */
    assert(cur.ptr == bytes);
    assert(cur.len == len);

    /* 5. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cur));
}
