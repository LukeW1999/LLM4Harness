#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 16
#endif

void aws_byte_buf_from_c_str_harness() {
    /* Non-deterministically allocate a null-terminated string */
    char *c_str = NULL;
    size_t str_len = 0;
    bool is_null = nondet_bool();

    if (!is_null) {
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_STRING_LEN);
        c_str = (char *)malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);
        __CPROVER_assume(__CPROVER_r_ok(c_str, str_len + 1));
        c_str[str_len] = '\0'; /* guaranteed null termination at end of allocated region */
    }

    /* Determine actual length of the string (may be less than str_len if earlier nulls) */
    size_t actual_len = (c_str != NULL) ? strlen(c_str) : 0;

    /* Save the contents of c_str before the call for immutability check */
    struct store_byte_from_buffer old_c_str;
    if (c_str != NULL && actual_len > 0) {
        save_byte_from_array((const uint8_t *)c_str, actual_len, &old_c_str);
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postcondition: returned buffer must be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Allocator is always NULL (non-owning view) */
    assert(buf.allocator == NULL);

    /* Capacity equals length */
    assert(buf.len == buf.capacity);

    if (c_str == NULL) {
        /* When input is NULL, output is empty */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);

        if (expected_len == 0) {
            /* Empty string results in NULL buffer */
            assert(buf.buffer == NULL);
        } else {
            /* Non-empty string: buffer points directly to c_str */
            assert(buf.buffer == (uint8_t *)c_str);
        }
        /* Ensure the source string was not modified */
        if (actual_len > 0) {
            assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str);
        }
    }
}
