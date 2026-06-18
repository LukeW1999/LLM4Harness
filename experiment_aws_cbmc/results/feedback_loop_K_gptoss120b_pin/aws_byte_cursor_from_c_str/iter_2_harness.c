#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondet pointer for c_str */
    const char *c_str = NULL;

    /* decide nondeterministically whether c_str is NULL */
    _Bool c_str_is_null = nondet_bool();
    __CPROVER_assume(c_str_is_null);
    if (!c_str_is_null) {
        /* allocate a buffer for the string */
        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= MAX_STRING_LEN);
        __CPROVER_assume(buf_len > 0);
        char *buf = aws_mem_acquire(allocator, buf_len);
        __CPROVER_assume(buf != NULL);

        /* fill buffer with nondet data */
        for (size_t i = 0; i < buf_len; ++i) {
            buf[i] = (char)nondet_uint8_t();
        }

        /* ensure there is a null terminator somewhere within the buffer */
        size_t null_pos = nondet_size_t();
        __CPROVER_assume(null_pos < buf_len);
        buf[null_pos] = '\0';

        c_str = buf;
    }

    if (c_str != NULL) {
        size_t original_len = strlen(c_str);
        char *original_copy = aws_mem_acquire(allocator, original_len + 1);
        __CPROVER_assume(original_copy != NULL);
        memcpy(original_copy, c_str, original_len + 1);
        const char *orig_ptr = original_copy;

        struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

        /* Postcondition: validity */
        assert(aws_byte_cursor_is_valid(&cur));

        /* Postcondition: pointer equality */
        assert(cur.ptr == (const uint8_t *)c_str);

        /* Postcondition: length */
        assert(cur.len == original_len);

        /* Frame condition: original memory unchanged */
        assert(memcmp(c_str, orig_ptr, original_len + 1) == 0);

        aws_mem_release(allocator, (void *)orig_ptr);
        aws_mem_release(allocator, (void *)c_str);
    } else {
        struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

        /* Postcondition: validity */
        assert(aws_byte_cursor_is_valid(&cur));

        /* Postcondition: pointer equality */
        assert(cur.ptr == NULL);

        /* Postcondition: length */
        assert(cur.len == 0);
    }
}
