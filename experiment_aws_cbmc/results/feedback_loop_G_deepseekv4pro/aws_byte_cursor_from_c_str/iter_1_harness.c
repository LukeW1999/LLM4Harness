#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness() {
    /* Non-deterministically choose NULL or a valid string */
    char *c_str_ptr = NULL;
    size_t len = 0;

    if (nondet_bool()) {
        /* NULL case */
        c_str_ptr = NULL;
        len = 0;
    } else {
        /* Non-NULL bounded string case */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        c_str_ptr = malloc(len + 1);
        if (c_str_ptr != NULL) {
            /* Ensure null termination */
            c_str_ptr[len] = '\0';
            /* The bytes up to len can be any non-deterministic value */
        }
    }

    /* Compute expected results */
    uint8_t *expected_ptr = (uint8_t *)c_str_ptr;
    size_t expected_len = (c_str_ptr != NULL) ? len : 0;

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_c_str(c_str_ptr);

    /* Postconditions: ptr and len match expectations */
    assert(cursor.ptr == expected_ptr);
    assert(cursor.len == expected_len);

    /* Validity invariant: the returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Clean up allocated memory */
    free(c_str_ptr);
}
