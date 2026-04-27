#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf1;
    struct aws_byte_buf buf2;
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    uint8_t buffer1[MAX_BUFFER_SIZE];
    uint8_t buffer2[MAX_BUFFER_SIZE];
    buf1.buffer = buffer1;
    buf2.buffer = buffer2;
    buf1.len = len;
    buf2.len = len;
    buf1.capacity = len;
    buf2.capacity = len;
    __CPROVER_assume(buf1.allocator == NULL);
    __CPROVER_assume(buf2.allocator == NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    /* 2. Non-deterministically set buffer contents */
    for (size_t i = 0; i < len; i++) {
        buffer1[i] = nondet_uint8();
        buffer2[i] = nondet_uint8();
    }

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq(&buf1, &buf2);

    /* 4. Assert postconditions */
    if (len == 0) {
        assert(result == true);
    } else {
        assert(result == (memcmp(buffer1, buffer2, len) == 0));
    }
}
