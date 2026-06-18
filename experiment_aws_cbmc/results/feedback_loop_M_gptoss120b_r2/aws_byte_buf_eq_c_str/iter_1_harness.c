#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_buf_eq_c_str_harness(void) {
    /* 1. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old = buf;

    /* 2. Nondeterministically create a C string (may be NULL) */
    bool c_str_is_null = nondet_bool();
    char *c_str = NULL;
    size_t c_str_len = 0;

    if (!c_str_is_null) {
        c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Fill with nondet bytes */
        for (size_t i = 0; i < c_str_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_str_len] = '\0';
    }

    /* 3. Call the function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* 4. Post‑condition: result must match the underlying array‑compare helper */
    bool expected = aws_array_eq_c_str(buf.buffer, buf.len, c_str);
    assert(result == expected);

    /* 5. Unchanged fields (the function is pure) */
    assert(buf.buffer   == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.len      == old.len);

    /* 6. Validity invariant must still hold */
    assert(aws_byte_buf_is_valid(&buf));
}
