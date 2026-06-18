#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_cursor_from_array_harness(void) {
    /* nondeterministic length */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate input buffer */
    uint8_t *buf = NULL;
    if (len > 0) {
        buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }
    }

    /* make a copy of the input buffer to check frame condition */
    uint8_t *buf_copy = NULL;
    if (buf != NULL) {
        buf_copy = malloc(len);
        __CPROVER_assume(buf_copy != NULL);
        memcpy(buf_copy, buf, len);
    }

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(buf, len);

    /* postconditions */

    /* 1. validity predicate */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 2. length invariants */
    assert(cur.len == len);
    assert(cur.ptr == buf);

    /* 3. frame condition: input buffer unchanged */
    if (buf != NULL) {
        assert(memcmp(buf, buf_copy, len) == 0);
    }

    /* clean up */
    free(buf);
    free(buf_copy);
    return 0;
}
