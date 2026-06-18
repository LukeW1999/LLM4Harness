#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024); /* bound to keep allocation reasonable */

    /* nondeterministic pointer to bytes */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* make the allocated memory fresh */
        __CPROVER_assume(__CPROVER_is_fresh(bytes, len));
        /* optionally initialize the buffer with nondet values */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    } else {
        /* when length is zero, pointer may be NULL */
        bytes = NULL;
    }

    /* PRE‑CALL SNAPSHOT */
    size_t old_len = len;
    uint8_t *old_ptr = bytes;
    /* copy of the original contents (if any) */
    uint8_t *old_contents = NULL;
    if (old_len > 0) {
        old_contents = malloc(old_len);
        __CPROVER_assume(old_contents != NULL);
        __CPROVER_assume(__CPROVER_is_fresh(old_contents, old_len));
        for (size_t i = 0; i < old_len; ++i) {
            old_contents[i] = old_ptr[i];
        }
    }

    /* CALL */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array((const void *)bytes, len);

    

    /* clean up */
    if (bytes) {
        free(bytes);
    }
    if (old_contents) {
        free(old_contents);
    }
}
