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
    /* 1. Declare and bound the aws_byte_buf */
    struct aws_byte_buf buf;
    buf.allocator = aws_default_allocator();

    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    if (buf.capacity > 0) {
        buf.buffer = malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
    } else {
        buf.buffer = NULL;
    }

    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old = buf;

    /* 2. Allocate a nondeterministic null‑terminated C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0'; /* ensure termination */

    /* 3. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 4. Post‑conditions: unchanged fields */
    assert(buf.buffer == old.buffer);
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* 5. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* 6. Result must match the underlying array comparison */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));
}
