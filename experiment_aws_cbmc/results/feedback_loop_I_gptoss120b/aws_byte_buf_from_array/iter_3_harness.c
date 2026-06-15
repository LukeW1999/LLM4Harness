#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_from_array_harness(void) {
    /* Destination byte buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = NULL;

    /* Save old state for frame condition checks */
    struct aws_byte_buf old_dest = dest;

    /* Nondeterministic source array and length */
    const uint8_t *src_bytes = nondet_uint8_t_ptr();
    size_t src_len = nondet_size_t();
    __CPROVER_assume(src_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src_len == 0 || src_bytes != NULL);

    /* Call function under test */
    int result = aws_byte_buf_from_array(&dest, src_bytes, src_len);

    /* Return value must be success */
    assert(result == AWS_OP_SUCCESS);

    /* Post‑conditions */
    assert(dest.allocator == NULL);
    assert(dest.len == src_len);
    assert(dest.capacity == src_len);
    if (src_len == 0) {
        assert(dest.buffer == NULL);
    } else {
        assert(dest.buffer == src_bytes);
        /* The referenced memory must remain unchanged */
        assert_bytes_match((uint8_t *)src_bytes, (uint8_t *)src_bytes, src_len);
    }

    /* dest must be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    /* Frame condition: allocator unchanged */
    assert(old_dest.allocator == dest.allocator);
}
