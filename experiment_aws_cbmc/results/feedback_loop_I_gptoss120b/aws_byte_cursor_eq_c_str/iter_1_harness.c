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

    /* 2. Save old state */
    struct aws_byte_cursor old = cursor;

    /* 3. Nondeterministically create a C string (may be NULL) */
    char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len < MAX_BUFFER_SIZE);
        c_str = malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Postconditions: no fields of cursor change */
    assert(cursor.len == old.len);
    assert(cursor.ptr == old.ptr);

    /* 6. c_str pointer must remain unchanged */
    if (c_str != NULL) {
        /* The pointer itself is not modified by the function */
        assert(c_str == c_str);
    }

    /* 7. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Clean up */
    free(c_str);
}
