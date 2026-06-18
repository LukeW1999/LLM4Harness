#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Declare input: a non-deterministic C string (may be NULL or valid) */
    const char *c_str;
    
    /* Non-deterministically choose between NULL and a valid string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Create a bounded non-deterministic string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator exists somewhere within bounds */
        buf[len] = '\0';
        c_str = buf;
    }

    /* 2. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* 3. Assert postconditions */
    
    /* If c_str is NULL, ptr should be NULL and len should be 0 */
    if (c_str == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If c_str is non-NULL, ptr should point to c_str and len should be strlen(c_str) */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
