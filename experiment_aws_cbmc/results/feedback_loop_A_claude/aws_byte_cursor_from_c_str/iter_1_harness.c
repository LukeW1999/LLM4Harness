#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <stddef.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Declare and set up a non-deterministic null-terminated string */
    /* We use a bounded array to keep the state space manageable */
    const char *c_str;

    /* Non-deterministically choose between NULL and a valid string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Create a bounded c-string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        buf[len] = '\0';
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* 3. Assert postconditions */

    /* The returned cursor's ptr should match the input c_str cast to uint8_t* */
    assert(result.ptr == (uint8_t *)c_str);

    /* If c_str is NULL, len should be 0 */
    if (c_str == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If c_str is non-NULL, len should be strlen(c_str) */
        assert(result.len == strlen(c_str));
        assert(result.ptr != NULL);
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
