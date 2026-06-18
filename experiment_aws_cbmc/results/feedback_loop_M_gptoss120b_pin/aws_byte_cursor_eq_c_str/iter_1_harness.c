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

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save old state */
    struct aws_byte_cursor old = cursor;

    /* 2. Create nondet null‑terminated string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    bool c_str_is_null = nondet_bool();

    const char *c_str = NULL;
    if (!c_str_is_null) {
        char *tmp = malloc(c_str_len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < c_str_len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[c_str_len] = '\0';
        c_str = tmp;
    } else {
        c_str = NULL;
        c_str_len = 0;
    }

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Postconditions */
    /* Result must equal the underlying array comparison */
    assert(result == aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));

    /* Unchanged fields */
    assert(cursor.len == old.len);
    assert(cursor.ptr == old.ptr);
    assert(cursor.allocator == old.allocator);

    /* 5. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));
}
