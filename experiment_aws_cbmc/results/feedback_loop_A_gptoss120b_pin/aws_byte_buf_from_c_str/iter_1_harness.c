#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministic input string (may be NULL) */
    char *c_str;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
    } else {
        /* bound the length of the string */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate space for the string plus terminating NUL */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL); /* allocation must succeed for this path */

        /* fill with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[len] = '\0';
    }

    /* call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* post‑condition checks */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        assert(result.len == strlen(c_str));
        assert(result.capacity == result.len);
        assert(result.buffer == (uint8_t *)c_str);
    }
    assert(result.allocator == NULL);

    /* validity invariant must hold */
    assert(aws_byte_buf_is_valid(&result));
}
