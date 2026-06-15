#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* nondeterministically decide whether buffers should be equal */
    bool make_equal = __CPROVER_nondet_bool();

    if (make_equal) {
        /* make lengths equal */
        if (a.capacity < b.capacity) {
            b.len = a.len;
        } else {
            a.len = b.len;
        }

        /* ensure both have enough capacity for the chosen length */
        if (a.len > a.capacity) a.len = a.capacity;
        if (b.len > b.capacity) b.len = b.capacity;

        /* copy bytes from a to b */
        for (size_t i = 0; i < a.len; ++i) {
            b.buffer[i] = a.buffer[i];
        }
    } else {
        /* make buffers unequal: either length differs or a byte differs */
        bool diff_len = __CPROVER_nondet_bool();

        if (diff_len) {
            /* force length difference while staying within capacity */
            if (a.len < a.capacity) {
                a.len = a.len + 1;
            } else if (b.len < b.capacity) {
                b.len = b.len + 1;
            } else {
                a.len = 0;
                b.len = 1;
            }
        } else {
            /* same length but at least one differing byte */
            if (a.len == 0 && a.capacity > 0 && b.capacity > 0) {
                a.len = 1;
                b.len = 1;
            } else {
                b.len = a.len;
            }

            if (a.len > 0 && a.buffer != NULL && b.buffer != NULL) {
                /* copy all but first byte */
                for (size_t i = 1; i < a.len; ++i) {
                    b.buffer[i] = a.buffer[i];
                }
                /* make first byte differ */
                b.buffer[0] = a.buffer[0] ^ 0xFF;
            }
        }
    }

    bool result = aws_byte_buf_eq(&a, &b);

    /* compute expected result */
    bool expected = false;
    if (a.len == b.len) {
        bool match = true;
        for (size_t i = 0; i < a.len; ++i) {
            if (a.buffer[i] != b.buffer[i]) {
                match = false;
                break;
            }
        }
        expected = match;
    }

    assert(result == expected);

    /* buffers must remain valid after the call */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
