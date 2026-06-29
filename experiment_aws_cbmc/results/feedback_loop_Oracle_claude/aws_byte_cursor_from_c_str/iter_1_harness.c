#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* Nondeterministically choose whether to pass NULL or a valid c-string */
    bool is_null;
    __CPROVER_assume(true); /* no constraint needed, just declare */

    const char *c_str;

    if (is_null) {
        c_str = NULL;
    } else {
        /* Create a bounded-length null-terminated string */
        size_t len;
        __CPROVER_assume(len <= 32); /* bound for tractability */
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        buf[len] = '\0';
        c_str = buf;
    }

    /* Save a copy of the pointer before the call */
    const char *c_str_before = c_str;

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

    /* Postcondition 1: Validity predicate */
    assert(aws_byte_cursor_is_valid(&result));

    /* Postcondition 2: If c_str is NULL, ptr must be NULL and len must be 0 */
    if (c_str == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Postcondition 3: ptr must point to the same memory as c_str */
        assert(result.ptr == (uint8_t *)c_str);

        /* Postcondition 4: len must equal strlen(c_str) */
        assert(result.len == strlen(c_str));

        /* Postcondition 5: len invariant - len is consistent with the null-terminated string */
        /* The character at ptr[len] must be '\0' (null terminator) */
        assert(result.ptr[result.len] == '\0');
    }

    /* Postcondition 6: Frame condition - c_str pointer itself was not modified */
    assert(c_str == c_str_before);

    /* Postcondition 7: len must not be SIZE_MAX (sanity check for overflow) */
    assert(result.len != SIZE_MAX);

    /* Postcondition 8: If ptr is NULL then len is 0 */
    if (result.ptr == NULL) {
        assert(result.len == 0);
    }

    /* Postcondition 9: If len > 0 then ptr is not NULL */
    if (result.len > 0) {
        assert(result.ptr != NULL);
    }
}
