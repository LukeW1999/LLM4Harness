#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* Setup: use ground-truth preconditions exactly */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postcondition 1: After clean_up_secure, the buffer pointer should be NULL */
    assert(buf.buffer == NULL);

    /* Postcondition 2: After clean_up_secure, the length should be 0 */
    assert(buf.len == 0);

    /* Postcondition 3: After clean_up_secure, the capacity should be 0 */
    assert(buf.capacity == 0);

    /* Postcondition 4: After clean_up_secure, the allocator should be NULL */
    assert(buf.allocator == NULL);

    /* Postcondition 5: The resulting buffer should be valid (zeroed out state) */
    assert(aws_byte_buf_is_valid(&buf));
}
