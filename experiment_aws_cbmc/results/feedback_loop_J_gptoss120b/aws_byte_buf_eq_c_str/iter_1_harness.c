#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old = buf;

    /* 2. Allocate a nondeterministic C string (null‑terminated) */
    size_t max_str_len = MAX_BUFFER_SIZE;
    char *c_str = malloc(max_str_len);
    __CPROVER_assume(c_str != NULL);
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < max_str_len);
    /* make it a valid C string */
    c_str[str_len] = '\0';

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 4. Postconditions */

    /* The function must not modify any fields of the buffer */
    assert(buf.len == old.len);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);

    /* The return value must equal the result of the underlying array comparison */
    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));

    /* 5. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));

    /* Clean up */
    free(c_str);
}
