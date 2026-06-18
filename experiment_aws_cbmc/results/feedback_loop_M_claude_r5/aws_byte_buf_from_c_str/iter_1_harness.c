#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic c_str input */
    /* We need a bounded string to avoid infinite state space */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Either NULL or a valid string */
    bool use_null = nondet_bool();
    const char *c_str;

    if (use_null) {
        c_str = NULL;
    } else {
        /* Allocate a string of str_len characters plus null terminator */
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator at position str_len */
        buf[str_len] = '\0';
        /* The rest of the bytes are nondet (already nondet from malloc) */
        /* Ensure no embedded null terminators before str_len to make strlen == str_len */
        /* Actually, we can't easily guarantee this without a loop, so let's just
           use a simple approach: assume the string is valid with known length */
        __CPROVER_assume(strlen(buf) == str_len);
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions based on the implementation:
       - buf.len = (c_str == NULL) ? 0 : strlen(c_str)
       - buf.capacity = buf.len
       - buf.buffer = (capacity == 0) ? NULL : (uint8_t *)c_str
       - buf.allocator = NULL
    */

    if (c_str == NULL) {
        /* NULL input: len=0, capacity=0, buffer=NULL, allocator=NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
        assert(result.allocator == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.allocator == NULL);

        if (expected_len == 0) {
            /* Empty string: buffer should be NULL (capacity == 0) */
            assert(result.buffer == NULL);
        } else {
            /* Non-empty string: buffer points to c_str */
            assert(result.buffer == (uint8_t *)c_str);
        }
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
