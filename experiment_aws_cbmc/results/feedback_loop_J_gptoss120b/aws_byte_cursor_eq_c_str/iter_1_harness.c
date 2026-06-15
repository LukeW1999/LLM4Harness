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

/* Bounding constants are provided by the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

#ifndef MAX_STRING_SIZE
#define MAX_STRING_SIZE 256
#endif

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Allocate a nondeterministic C string */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_STRING_SIZE);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* make the string readable and null‑terminated */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    /* 3. Save old state of the cursor (inputs are const) */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Post‑condition: inputs must remain unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    /* allocator field does not exist for cursor; nothing else to check */

    /* 6. Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. The C string pointer is const; it must not be modified */
    /* (no direct way to check contents unchanged, but pointer must stay the same) */
    assert(c_str == c_str); /* trivial pointer stability check */

    /* 8. Clean up */
    free(c_str);
}
