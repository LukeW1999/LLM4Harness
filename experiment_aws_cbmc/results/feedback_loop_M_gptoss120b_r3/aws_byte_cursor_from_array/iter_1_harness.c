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
    /* 1. Nondeterministic length bounded by MAX_BUFFER_SIZE */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Allocate a readable buffer when length is non‑zero */
    const void *bytes = NULL;
    if (len > 0) {
        uint8_t *tmp = malloc(len);
        __CPROVER_assume(AWS_MEM_IS_READABLE(tmp, len));
        bytes = tmp;
    }

    /* 3. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* 4. Post‑condition asserts */
    assert(cur.len == len);
    assert(cur.ptr == (uint8_t *)bytes);
    assert(aws_byte_cursor_is_valid(&cur));
}
