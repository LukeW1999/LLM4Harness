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
    /* 1. Non‑deterministic length bounded by the proof constant */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a buffer of the chosen length (or NULL if length is zero) */
    uint8_t *buf = NULL;
    if (len > 0) {
        buf = malloc(len);
        /* Assume allocation succeeded for the purpose of this harness */
        __CPROVER_assume(buf != NULL);
    }

    const void *bytes = buf;

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Post‑conditions: fields must match the inputs */
    assert(cur.len == len);
    assert(cur.ptr == (uint8_t *)bytes);

    /* 5. Validity invariant must hold for the returned cursor */
    assert(aws_byte_cursor_is_valid(&cur));
}
