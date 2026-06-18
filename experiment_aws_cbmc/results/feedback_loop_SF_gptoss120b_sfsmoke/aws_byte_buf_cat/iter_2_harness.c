#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* dest buffer */
    struct aws_byte_buf dest;
    dest.allocator = alloc;
    dest.capacity = nondet_size_t();
    dest.buffer = malloc(dest.capacity);
    __CPROVER_assume(dest.buffer != NULL || dest.capacity == 0);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* number of arguments (constrained to 2 for the harness) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2);
    __CPROVER_assume(number_of_args == 2);

    /* source buffer 0 */
    struct aws_byte_buf src0;
    src0.allocator = alloc;
    src0.capacity = nondet_size_t();
    src0.buffer = malloc(src0.capacity);
    __CPROVER_assume(src0.buffer != NULL || src0.capacity == 0);
    src0.len = nondet_size_t();
    __CPROVER_assume(src0.len <= src0.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

    /* source buffer 1 */
    struct aws_byte_buf src1;
    src1.allocator = alloc;
    src1.capacity = nondet_size_t();
    src1.buffer = malloc(src1.capacity);
    __CPROVER_assume(src1.buffer != NULL || src1.capacity == 0);
    src1.len = nondet_size_t();
    __CPROVER_assume(src1.len <= src1.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    /* pre‑call snapshot of dest */
    size_t dest_len_old = dest.len;
    size_t dest_cap_old = dest.capacity;
    uint8_t *dest_buf_old = dest.buffer;

    /* call under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, &src0, &src1);

    /* dest must remain a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));
    /* allocator must not change */
    assert(dest.allocator == alloc);
    /* capacity must be at least the length */
    assert(dest.capacity >= dest.len);
    /* buffer pointer must be non‑null when capacity is non‑zero */
    assert(dest.buffer != NULL || dest.capacity == 0);

    if (result == AWS_OP_SUCCESS) {
        /* on success, length is increased by the sum of source lengths */
        assert(dest.len == dest_len_old + src0.len + src1.len);
    } else {
        /* on failure, length may have changed but must stay within bounds */
        assert(dest.len >= dest_len_old);
        assert(dest.len <= dest_len_old + src0.len + src1.len);
    }
}
