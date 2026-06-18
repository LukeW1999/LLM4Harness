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
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save old state of the cursor */
    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer old_cursor_bytes;
    save_byte_from_array(cursor.ptr, cursor.len, &old_cursor_bytes);

    /* 2. Create a nondeterministic null‑terminated C string */
    size_t c_len = nondet_size_t();
    __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < c_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[c_len] = '\0';

    /* Save old state of the C string */
    struct store_byte_from_buffer old_c_str_bytes;
    save_byte_from_array((uint8_t *)c_str, c_len, &old_c_str_bytes);

    /* 3. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Post‑conditions – the function must not modify any input */
    /* cursor fields unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.capacity == old_cursor.capacity);
    /* cursor buffer contents unchanged */
    assert_bytes_match(cursor.ptr, old_cursor.ptr, cursor.len);
    /* c_str contents unchanged */
    assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_str_bytes);

    /* 5. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
}
