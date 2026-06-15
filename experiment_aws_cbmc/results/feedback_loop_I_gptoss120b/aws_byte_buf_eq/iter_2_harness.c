#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    /* 1. Declare and bound input structures */
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 2. Save old state for frame condition */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* 4. Frame condition: inputs must remain unchanged */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);
    if (a.buffer != NULL) {
        assert_bytes_match(a.buffer, old_a.buffer, a.len);
    }

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);
    if (b.buffer != NULL) {
        assert_bytes_match(b.buffer, old_b.buffer, b.len);
    }

    /* 5. Result correctness */
    bool expected = true;
    if (a.len != b.len) {
        expected = false;
    } else {
        size_t i;
        for (i = 0; i < a.len; ++i) {
            if (a.buffer[i] != b.buffer[i]) {
                expected = false;
                break;
            }
        }
    }
    assert((result != false) == expected);

    /* 6. Invariant: both structures must be valid after the call */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
