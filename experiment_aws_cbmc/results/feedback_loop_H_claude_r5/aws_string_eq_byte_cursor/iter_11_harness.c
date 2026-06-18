#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

#ifndef MAX_CURSOR_LEN
#define MAX_CURSOR_LEN 8
#endif

void aws_string_eq_byte_cursor_harness(void) {
    /* Allocate a valid aws_string */
    const struct aws_string *str = ensure_string_is_allocated_bounded(MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Allocate a valid aws_byte_cursor with readable memory */
    struct aws_byte_cursor cur;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_CURSOR_LEN);

    /* Allocate buffer large enough for cur.len bytes */
    /* Use MAX_CURSOR_LEN + 1 to avoid malloc(0) issues and ensure readability */
    uint8_t *buf = malloc(MAX_CURSOR_LEN + 1);
    __CPROVER_assume(buf != NULL);
    cur.ptr = (cur.len == 0) ? NULL : buf;

    /* For zero-length cursor, ptr can be NULL */
    /* For non-zero length, ptr must be non-NULL and readable */
    if (cur.len > 0) {
        __CPROVER_assume(cur.ptr != NULL);
        /* Ensure the buffer is readable for cur.len bytes */
        __CPROVER_assume(cur.len <= MAX_CURSOR_LEN);
    }

    __CPROVER_assume(aws_byte_cursor_is_valid(&cur));

    /* Save old state for immutability checks */
    size_t old_str_len = str->len;
    struct aws_allocator *old_str_allocator = str->allocator;

    size_t old_cur_len = cur.len;
    uint8_t *old_cur_ptr = cur.ptr;

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, &cur);

    /* Postcondition: str fields unchanged (immutability) */
    assert(str->len == old_str_len);
    assert(str->allocator == old_str_allocator);

    /* Postcondition: cur fields unchanged (immutability) */
    assert(cur.len == old_cur_len);
    assert(cur.ptr == old_cur_ptr);

    /* Postcondition: Validity invariants still hold */
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_valid(&cur));
}
