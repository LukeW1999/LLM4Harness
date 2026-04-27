#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_byte_buf_from_array_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    const uint8_t *array;
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    array = malloc(array_len);
    __CPROVER_assume(array != NULL);

    /* 2. Initialize destination buffer */
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = can_fail_allocator();

    /* 3. Call function under test */
    aws_byte_buf_from_array(&buf, array, array_len);

    /* 4. Assert postconditions */
    assert(buf.len == array_len);
    assert(buf.capacity == array_len);
    assert(buf.buffer == array);
    assert(memcmp(buf.buffer, array, array_len) == 0);
    assert(buf.allocator == can_fail_allocator());

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));

    /* 6. Free allocated memory */
    free((void *)array);
}
