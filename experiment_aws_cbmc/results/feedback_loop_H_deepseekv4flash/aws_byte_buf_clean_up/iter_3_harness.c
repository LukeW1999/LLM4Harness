#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    make_aws_byte_buf(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    struct aws_byte_buf old = buf;

    aws_byte_buf_clean_up(&buf);

    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == old.allocator);
}
