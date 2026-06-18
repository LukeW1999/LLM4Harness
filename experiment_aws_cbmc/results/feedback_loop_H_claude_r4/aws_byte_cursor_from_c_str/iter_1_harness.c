#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Declare input: a non-deterministic C string (may be NULL or non-NULL) */
    const char *c_str;

    /* Non-deterministically choose whether c_str is NULL or points to valid memory */
    bool is_null = nondet_bool();
    if (is_null) {
        c_str = NULL;
    } else {
        /* Create a bounded null-terminated string */
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

    /* From the implementation:
     * cur.ptr = (uint8_t *)c_str;
     * cur.len = (cur.ptr) ? strlen(c_str) : 0;
     */

    /* ptr field: always set to c_str cast to uint8_t* */
    assert(result.ptr == (uint8_t *)c_str);

    /* len field: 0 if ptr is NULL, strlen(c_str) otherwise */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.ptr == NULL);
    } else {
        assert(result.len == strlen(c_str));
        assert(result.ptr != NULL);
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
