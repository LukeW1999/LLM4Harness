#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>

/**
 * Harness for aws_byte_buf_from_c_str
 *
 * The function:
 * - Takes a null-terminated C string (or NULL)
 * - Returns an aws_byte_buf by value
 * - buf.len = strlen(c_str) if c_str != NULL, else 0
 * - buf.capacity = buf.len
 * - buf.buffer = (uint8_t*)c_str if len > 0, else NULL
 * - buf.allocator = NULL
 */
void aws_byte_buf_from_c_str_harness() {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    const char *c_str;

    if (nondet_bool()) {
        /* NULL input case */
        c_str = NULL;
    } else {
        /* Non-null string: allocate a bounded string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        /* Allocate len+1 bytes for the string including null terminator */
        char *str = malloc(len + 1);
        __CPROVER_assume(str != NULL);
        /* Ensure null terminator is present */
        str[len] = '\0';
        c_str = str;
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Postconditions */
    if (c_str == NULL) {
        /* NULL input: len=0, capacity=0, buffer=NULL, allocator=NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        /* len equals strlen(c_str) */
        assert(buf.len == expected_len);
        /* capacity equals len */
        assert(buf.capacity == expected_len);
        /* allocator is NULL */
        assert(buf.allocator == NULL);
        if (expected_len == 0) {
            /* Empty string: buffer is NULL */
            assert(buf.buffer == NULL);
        } else {
            /* Non-empty string: buffer points to c_str */
            assert(buf.buffer == (uint8_t *)c_str);
        }
    }

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
