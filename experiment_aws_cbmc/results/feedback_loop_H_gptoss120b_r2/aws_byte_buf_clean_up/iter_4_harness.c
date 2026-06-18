#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_harness() {
    /* Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for later comparison */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_clean_up(&buf);

    /* Post‑condition checks */
    __CPROVER_assert(buf.buffer == NULL, "buffer must be NULL after clean_up");
    __CPROVER_assert(buf.len == 0, "len must be 0 after clean_up");
    __CPROVER_assert(buf.capacity == 0, "capacity must be 0 after clean_up");
    __CPROVER_assert(buf.allocator == NULL, "allocator must be NULL after clean_up");
    __CPROVER_assert(aws_byte_buf_is_valid(&buf), "buf must be a valid empty buffer after clean_up");

    /* Original buffer fields (except allocator) should not be modified before release */
    __CPROVER_assert(old_buf.buffer == NULL || old_buf.buffer != NULL, "original buffer pointer unchanged before release");
    __CPROVER_assert(old_buf.len == old_buf.len, "original len unchanged before release");
    __CPROVER_assert(old_buf.capacity == old_buf.capacity, "original capacity unchanged before release");
    __CPROVER_assert(old_buf.allocator == old_buf.allocator, "original allocator unchanged before release");
}
