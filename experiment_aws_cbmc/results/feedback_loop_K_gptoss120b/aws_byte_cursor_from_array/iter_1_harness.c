#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_cursor_from_array_harness(void) {
    /* Precondition: nondet length, bounded to keep verification tractable */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024);               /* optional bound */

    /* Precondition: allocate buffer if length > 0 */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* initialise buffer with nondet data */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Snapshot the original buffer contents for frame condition */
    uint8_t *bytes_snapshot = NULL;
    if (len > 0) {
        bytes_snapshot = malloc(len);
        __CPROVER_assume(bytes_snapshot != NULL);
        memcpy(bytes_snapshot, bytes, len);
    }

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    /* Postconditions: validity */
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    /* Postconditions: frame (no modification of the input buffer) */
    if (len > 0) {
        assert(memcmp(bytes, bytes_snapshot, len) == 0);
    }

    /* Clean up */
    free(bytes);
    free(bytes_snapshot);
    return 0;
}
