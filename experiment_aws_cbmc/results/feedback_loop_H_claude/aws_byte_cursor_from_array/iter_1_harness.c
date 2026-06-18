#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_array_harness(void) {
    /* 1. Declare inputs */
    size_t len = nondet_size_t();
    void *bytes = NULL;

    /* If len > 0, we need a valid pointer; if len == 0, NULL is allowed */
    if (len > 0) {
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* 2. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* 3. Assert postconditions */
    /* The function sets cur.ptr = (uint8_t *)bytes and cur.len = len */
    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);

    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
