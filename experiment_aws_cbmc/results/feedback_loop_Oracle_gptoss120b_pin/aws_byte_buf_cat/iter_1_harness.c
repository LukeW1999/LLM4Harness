#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256U
#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR    1

void aws_byte_buf_cat_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* destination buffer */
    struct aws_byte_buf dest;
    dest.allocator = alloc;
    dest.capacity = nondet_uint();
    __CPROVER_assume(dest.capacity <= MAX_BUFFER_SIZE);
    dest.buffer = (uint8_t *)malloc(dest.capacity);
    __CPROVER_assume(dest.buffer != NULL);
    dest.len = nondet_uint();
    __CPROVER_assume(dest.len <= dest.capacity);

    /* source buffers */
    struct aws_byte_buf buffer1, buffer2, buffer3;

    buffer1.allocator = alloc;
    buffer1.capacity = nondet_uint();
    __CPROVER_assume(buffer1.capacity <= MAX_BUFFER_SIZE);
    buffer1.buffer = (uint8_t *)malloc(buffer1.capacity);
    __CPROVER_assume(buffer1.buffer != NULL);
    buffer1.len = nondet_uint();
    __CPROVER_assume(buffer1.len <= buffer1.capacity);

    buffer2.allocator = alloc;
    buffer2.capacity = nondet_uint();
    __CPROVER_assume(buffer2.capacity <= MAX_BUFFER_SIZE);
    buffer2.buffer = (uint8_t *)malloc(buffer2.capacity);
    __CPROVER_assume(buffer2.buffer != NULL);
    buffer2.len = nondet_uint();
    __CPROVER_assume(buffer2.len <= buffer2.capacity);

    buffer3.allocator = alloc;
    buffer3.capacity = nondet_uint();
    __CPROVER_assume(buffer3.capacity <= MAX_BUFFER_SIZE);
    buffer3.buffer = (uint8_t *)malloc(buffer3.capacity);
    __CPROVER_assume(buffer3.buffer != NULL);
    buffer3.len = nondet_uint();
    __CPROVER_assume(buffer3.len <= buffer3.capacity);

    /* structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer1));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer2));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer3, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer3));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* nondet number of arguments (1..3) */
    size_t number_of_args = nondet_uint();
    __CPROVER_assume(number_of_args >= 1 && number_of_args <= 3);

    /* save original state for post‑condition checks */
    size_t orig_dest_len = dest.len;
    uint8_t *orig_dest_buf = dest.buffer;
    uint8_t *dest_copy = malloc(dest.capacity);
    __CPROVER_assume(dest_copy != NULL);
    memcpy(dest_copy, dest.buffer, dest.capacity);

    uint8_t *buf1_copy = malloc(buffer1.capacity);
    __CPROVER_assume(buf1_copy != NULL);
    memcpy(buf1_copy, buffer1.buffer, buffer1.capacity);

    uint8_t *buf2_copy = malloc(buffer2.capacity);
    __CPROVER_assume(buf2_copy != NULL);
    memcpy(buf2_copy, buffer2.buffer, buffer2.capacity);

    uint8_t *buf3_copy = malloc(buffer3.capacity);
    __CPROVER_assume(buf3_copy != NULL);
    memcpy(buf3_copy, buffer3.buffer, buffer3.capacity);

    /* compute expected total length of data to be concatenated */
    size_t expected_total = 0;
    if (number_of_args >= 1) expected_total += buffer1.len;
    if (number_of_args >= 2) expected_total += buffer2.len;
    if (number_of_args >= 3) expected_total += buffer3.len;

    /* call the function under verification */
    int ret = aws_byte_buf_cat(&dest, number_of_args,
                               &buffer1,
                               &buffer2,
                               &buffer3);

    /* post‑condition 1: return value respects success/failure */
    if (ret == AWS_OP_SUCCESS) {
        /* on success the whole concatenation must have been performed */
        assert(dest.len == orig_dest_len + expected_total);
    } else {
        /* on error the length cannot exceed what could have been appended */
        assert(dest.len <= orig_dest_len + expected_total);
    }

    /* post‑condition 2: destination buffer invariants */
    assert(dest.buffer == orig_dest_buf);               /* no reallocation */
    assert(dest.capacity >= dest.len);                  /* capacity not violated */
    assert(aws_byte_buf_is_valid(&dest));              /* structural validity */

    /* post‑condition 3: source buffers unchanged */
    assert(aws_byte_buf_is_valid(&buffer1));
    assert(aws_byte_buf_is_valid(&buffer2));
    assert(aws_byte_buf_is_valid(&buffer3));
    assert(memcmp(buffer1.buffer, buf1_copy, buffer1.capacity) == 0);
    assert(memcmp(buffer2.buffer, buf2_copy, buffer2.capacity) == 0);
    assert(memcmp(buffer3.buffer, buf3_copy, buffer3.capacity) == 0);

    /* post‑condition 4: data correctly concatenated on success */
    if (ret == AWS_OP_SUCCESS) {
        size_t offset = orig_dest_len;
        if (number_of_args >= 1) {
            assert(memcmp(dest.buffer + offset, buffer1.buffer, buffer1.len) == 0);
            offset += buffer1.len;
        }
        if (number_of_args >= 2) {
            assert(memcmp(dest.buffer + offset, buffer2.buffer, buffer2.len) == 0);
            offset += buffer2.len;
        }
        if (number_of_args >= 3) {
            assert(memcmp(dest.buffer + offset, buffer3.buffer, buffer3.len) == 0);
            offset += buffer3.len;
        }
    } else {
        /* on error the prefix that was successfully appended (if any) must match */
        size_t offset = orig_dest_len;
        size_t remaining = dest.len - orig_dest_len;
        if (number_of_args >= 1 && remaining > 0) {
            size_t copy = remaining < buffer1.len ? remaining : buffer1.len;
            assert(memcmp(dest.buffer + offset, buffer1.buffer, copy) == 0);
            offset += copy;
            remaining -= copy;
        }
        if (number_of_args >= 2 && remaining > 0) {
            size_t copy = remaining < buffer2.len ? remaining : buffer2.len;
            assert(memcmp(dest.buffer + offset, buffer2.buffer, copy) == 0);
            offset += copy;
            remaining -= copy;
        }
        if (number_of_args >= 3 && remaining > 0) {
            size_t copy = remaining < buffer3.len ? remaining : buffer3.len;
            assert(memcmp(dest.buffer + offset, buffer3.buffer, copy) == 0);
        }
    }

    /* clean up */
    free(dest_copy);
    free(buf1_copy);
    free(buf2_copy);
    free(buf3_copy);
    free(dest.buffer);
    free(buffer1.buffer);
    free(buffer2.buffer);
    free(buffer3.buffer);

    return 0;
}
