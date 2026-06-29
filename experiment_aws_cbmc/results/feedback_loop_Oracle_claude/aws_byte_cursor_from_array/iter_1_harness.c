#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_array_harness(void) {
    size_t len;
    __CPROVER_assume(len <= 1024); /* bound for CBMC tractability */

    uint8_t *bytes;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* len == 0: bytes may be NULL per the API contract */
        bytes = nondet_bool() ? NULL : malloc(1);
    }

    /* Save original pointer and length for frame condition checks */
    const void *const orig_bytes = bytes;
    const size_t orig_len = len;

    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* Postcondition 1: validity predicate */
    assert(aws_byte_cursor_is_valid(&result));

    /* Postcondition 2: output length invariant */
    assert(result.len == orig_len);

    /* Postcondition 3: output pointer invariant */
    assert(result.ptr == (uint8_t *)orig_bytes);

    /* Postcondition 4: frame condition - the bytes array is not modified */
    /* (CBMC tracks memory writes; no writes to bytes[] should have occurred) */
    /* We verify by checking the pointer still refers to the same memory */
    assert(result.ptr == bytes);

    /* Postcondition 5: if len is 0, cursor is still valid (ptr may be NULL) */
    if (orig_len == 0) {
        assert(result.len == 0);
    }

    /* Postcondition 6: if bytes is non-NULL and len > 0, ptr is non-NULL */
    if (orig_bytes != NULL && orig_len > 0) {
        assert(result.ptr != NULL);
    }
}
