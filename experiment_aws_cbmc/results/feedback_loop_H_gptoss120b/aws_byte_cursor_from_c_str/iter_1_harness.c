#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "aws/common/byte_buf.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Non‑deterministically choose a length for the string (bounded) */
    size_t max_len = MAX_BUFFER_SIZE;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    /* 2. Non‑deterministically decide whether the pointer is NULL or points to a valid C string */
    const char *c_str = NULL;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* allocate space for the string plus the terminating NUL */
        char *tmp = malloc(len + 1);
        __CPROVER_assume(tmp != NULL);
        /* fill the buffer with nondeterministic bytes */
        for (size_t i = 0; i < len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        /* ensure NUL termination */
        tmp[len] = '\0';
        c_str = tmp;
    }

    /* 3. Save old state of the input pointer */
    const char *old_c_str = c_str;

    /* 4. Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* 5. Post‑conditions that must hold on every execution */
    /* The returned pointer must be the same as the input pointer (cast) */
    assert(cur.ptr == (uint8_t *)old_c_str);

    /* The length must be strlen(c_str) when c_str is non‑NULL, otherwise 0 */
    if (old_c_str) {
        size_t expected_len = strlen(old_c_str);
        assert(cur.len == expected_len);
    } else {
        assert(cur.len == 0);
    }

    /* The returned cursor must satisfy its validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 6. The input pointer itself must not be modified */
    assert(c_str == old_c_str);

    /* 7. Clean up any allocated memory */
    if (c_str) {
        free((void *)c_str);
    }
}
