#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_eq_harness(void) {
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    /* bound the buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    /* allocate internal arrays if needed */
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    /* validity predicates */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* nondeterministically decide whether the buffers should be equal */
    _Bool make_equal = __CPROVER_nondet_bool();

    if (make_equal) {
        /* make lengths equal and contents identical */
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= a.capacity);
        __CPROVER_assume(len <= b.capacity);
        a.len = len;
        b.len = len;

        if (len > 0 && a.buffer != NULL && b.buffer != NULL) {
            for (size_t i = 0; i < len; ++i) {
                unsigned char v = __CPROVER_nondet_uchar();
                a.buffer[i] = v;
                b.buffer[i] = v;
            }
        }
    } else {
        /* make them differ either by length or by content */
        _Bool diff_len = __CPROVER_nondet_bool();

        if (diff_len) {
            size_t len_a = __CPROVER_nondet_uint();
            size_t len_b = __CPROVER_nondet_uint();
            __CPROVER_assume(len_a <= a.capacity);
            __CPROVER_assume(len_b <= b.capacity);
            __CPROVER_assume(len_a != len_b);
            a.len = len_a;
            b.len = len_b;
        } else {
            /* same length but at least one byte differs; ensure length > 0 */
            size_t len = __CPROVER_nondet_uint();
            __CPROVER_assume(len > 0);
            __CPROVER_assume(len <= a.capacity);
            __CPROVER_assume(len <= b.capacity);
            a.len = len;
            b.len = len;

            if (a.buffer != NULL && b.buffer != NULL) {
                for (size_t i = 0; i < len; ++i) {
                    unsigned char v = __CPROVER_nondet_uchar();
                    a.buffer[i] = v;
                    b.buffer[i] = v;
                }
                /* ensure a differing byte */
                size_t idx = __CPROVER_nondet_uint();
                __CPROVER_assume(idx < len);
                b.buffer[idx] = b.buffer[idx] ^ 0xFF;
            }
        }
    }

    /* save old state for immutability checks */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* call function under test */
    int eq = aws_byte_buf_eq(&a, &b);

    /* buffers must remain unchanged */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);

    /* expected result */
    if (make_equal) {
        assert(eq == 1);
    } else {
        assert(eq == 0);
    }
}
