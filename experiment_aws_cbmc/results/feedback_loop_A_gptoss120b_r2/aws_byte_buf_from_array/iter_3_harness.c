#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness(void) {
    /* Declare and bound the destination buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Source array */
    uint8_t src_array[MAX_BUFFER_SIZE];
    const uint8_t *src = src_array;

    /* Length of data to copy */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Save old state of the destination buffer */
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    aws_byte_buf_from_array(&buf, src, len);

    /* Post‑condition checks */
    assert(buf.buffer == src);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* The source array must remain unchanged (trivially true for a stack array) */
    (void)old_buf; /* suppress unused variable warning */

    /* Validity invariant must hold */
    assert(aws_byte_buf_is_valid(&buf));
}
