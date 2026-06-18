#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor cursor;
    cursor.len = (size_t)nondet_uint();
    __CPROVER_assume(cursor.len <= 1024U);
    __CPROVER_assume(cursor.len > 0U);                     // ensure non‑zero allocation
    cursor.ptr = aws_mem_acquire(allocator, cursor.len);
    __CPROVER_assume(cursor.ptr != NULL);

    size_t c_str_len = (size_t)nondet_uint();
    __CPROVER_assume(c_str_len <= 1024U);
    char *c_str = aws_mem_acquire(allocator, c_str_len + 1U);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0';

    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;
    char *old_c_str = c_str;

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    assert(cursor.len == old_len);
    assert(cursor.ptr == old_ptr);
    assert(c_str == old_c_str);
    assert(c_str[c_str_len] == '\0');

    if (result) {
        assert(cursor.len == strlen(c_str));
        assert(memcmp(cursor.ptr, c_str, cursor.len) == 0);
    } else {
        assert(!(cursor.len == strlen(c_str) && memcmp(cursor.ptr, c_str, cursor.len) == 0));
    }
}
