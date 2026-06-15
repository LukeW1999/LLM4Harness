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

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Declare and bound the C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_len + 1));
    c_str[c_len] = '\0'; /* ensure null‑termination */

    /* 3. Save old state */
    struct aws_byte_buf old = buf;

    /* 4. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Unchanged fields (function is pure) */
    assert(buf.buffer == old.buffer);
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* 6. Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Clean up */
    free(c_str);
}
