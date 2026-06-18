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
    /* 1. Non‑deterministic inputs, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    const void *bytes = NULL;
    if (len > 0) {
        /* allocate a readable buffer of the requested length */
        uint8_t *tmp = malloc(len);
        __CPROVER_assume(AWS_MEM_IS_READABLE(tmp, len));
        bytes = tmp;
    }

    /* 2. Call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 3. Post‑conditions */
    /* The returned cursor must reflect the inputs */
    assert(cur.len == len);
    assert(cur.ptr == (uint8_t *)bytes);

    /* The cursor must satisfy its validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
