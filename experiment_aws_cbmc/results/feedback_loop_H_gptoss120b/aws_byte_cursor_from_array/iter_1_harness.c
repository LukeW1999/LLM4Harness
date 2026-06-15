#include <aws/common/byte_buf.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Non‑deterministic length bounded by the test harness limit */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a buffer of the chosen length (may be NULL when len == 0) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(AWS_MEM_IS_READABLE(bytes, len));
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Post‑conditions: the returned cursor must reflect the inputs */
    assert(cur.ptr == bytes);
    assert(cur.len == len);

    /* 5. The cursor must satisfy its validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));
}
