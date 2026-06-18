#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_LEN 256

void aws_byte_cursor_from_array_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_LEN);

    /* nondeterministic buffer */
    uint8_t *buf = NULL;
    uint8_t *buf_copy = NULL;

    if (len > 0) {
        buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }

        /* make a copy to check frame condition */
        buf_copy = malloc(len);
        __CPROVER_assume(buf_copy != NULL);
        memcpy(buf_copy, buf, len);
    } else {
        /* when length is zero the pointer may be NULL or any value;
           we simply keep it NULL for simplicity */
        buf = NULL;
        buf_copy = NULL;
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(buf, len);

    /* postcondition 1: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cur));

    /* postcondition 2: length invariants */
    assert(cur.len == len);
    assert(cur.ptr == buf);

    /* postcondition 3: frame condition – input memory unchanged */
    if (len > 0) {
        assert(memcmp(buf, buf_copy, len) == 0);
    }

    /* clean up */
    free(buf);
    free(buf_copy);
    return 0;
}
