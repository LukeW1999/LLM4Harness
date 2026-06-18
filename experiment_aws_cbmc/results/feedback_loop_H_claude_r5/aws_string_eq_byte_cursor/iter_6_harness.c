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

    /* Allocate a valid aws_byte_cursor with always-valid ptr */
    struct aws_byte_cursor cur;
    cur.len = nondet_size_t();
    __CPROVER_assume(cur.len <= MAX_CURSOR_LEN);

    /* Always allocate a buffer of at least 1 byte to ensure ptr is valid */
    size_t alloc_len = (cur.len > 0) ? cur.len : 1;
    cur.ptr = malloc(alloc_len);
    __CPROVER_assume(cur.ptr != NULL);

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
