#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_array_harness() {
    const void *bytes = nd_voidp();
    size_t len = nd_size_t();
    __CPROVER_assume(len <= MAX_MALLOC_SIZE);
    __CPROVER_assume(bytes != NULL || len == 0);

    struct aws_byte_cursor cur_old;
    cur_old.ptr = nd_uint8_ptr();
    cur_old.len = nd_size_t();

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    // Success path assertions
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    // Validity invariant
    assert(aws_byte_cursor_is_valid(&cur));

    // Failure path assertions (simulated by checking unchanged values)
    if (len == 0) {
        assert(cur.ptr == (uint8_t *)bytes); // Should point to the same location if len is 0
        assert(cur.len == len);
    }
}
