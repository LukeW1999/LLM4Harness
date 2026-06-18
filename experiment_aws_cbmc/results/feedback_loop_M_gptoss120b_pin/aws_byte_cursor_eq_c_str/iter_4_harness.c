#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256

static size_t nondet_size_t() {
    size_t x;
    return x;
}

static uint8_t nondet_uint8_t() {
    uint8_t x;
    return x;
}

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save old state */
    struct aws_byte_cursor old = cursor;

    /* 2. Create nondet null‑terminated string (must be non‑NULL) */
    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

    char *tmp = malloc(c_str_len + 1);
    __CPROVER_assume(tmp != NULL);
    for (size_t i = 0; i < c_str_len; ++i) {
        tmp[i] = (char)nondet_uint8_t();
    }
    tmp[c_str_len] = '\0';
    const char *c_str = tmp;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 4. Postconditions */
    assert(result == aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));

    /* Unchanged fields */
    assert(cursor.len == old.len);
    assert(cursor.ptr == old.ptr);

    /* 5. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));
}
