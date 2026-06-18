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

    /* nondet capacity within the bound */
    buf.capacity = nondet_size_t();
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);

    /* nondet length not exceeding capacity */
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    /* use the default allocator (required for validity) */
    buf.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Allocate a nondet null‑terminated C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_len] = '\0';

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;
    const char *old_c_str = c_str;

    /* 4. Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 5. Assert unchanged fields */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);
    assert(c_str == old_c_str);

    /* 6. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* 7. Assert functional correctness */
    bool expected = aws_array_eq_c_str(buf.buffer, buf.len, c_str);
    assert(result == expected);
}
