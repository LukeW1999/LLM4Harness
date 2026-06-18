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

    /* 3. Allocate a nondeterministic null‑terminated string */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make it a proper C string */
    c_str[str_len] = '\0';

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑condition: cursor must be unchanged */
    assert(cursor.len == old.len);
    assert(cursor.ptr == old.ptr);

    /* 6. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. Clean up */
    free(c_str);
}
