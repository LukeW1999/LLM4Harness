#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>

#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

/* Function prototype from aws/common/hash_table.h */
extern bool aws_ptr_eq(const void *a, const void *b);

/* Nondeterministic boolean generator */
bool nondet_bool(void);

void aws_ptr_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Allocate two byte buffers and bound them */
    struct aws_byte_buf buf1, buf2;

    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
    buf1.allocator = allocator;                     /* set allocator for validity */

    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));
    buf2.allocator = allocator;                     /* set allocator for validity */

    /* 2. Choose pointers for comparison */
    const void *a = buf1.buffer;
    const void *b;

    bool make_equal = nondet_bool();
    if (make_equal) {
        b = a;                                      /* pointers equal */
    } else {
        b = buf2.buffer;                           /* pointers distinct */
    }

    /* Save old state for immutability checks */
    struct aws_byte_buf old_buf1 = buf1;
    struct aws_byte_buf old_buf2 = buf2;
    const void *old_a = a;
    const void *old_b = b;

    /* 3. Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* 4. Post‑condition: result reflects pointer equality */
    assert(result == (old_a == old_b));

    /* 5. Unchanged fields: inputs and buffers must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);
    assert(buf1.len == old_buf1.len);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.buffer == old_buf1.buffer);
    assert(buf1.allocator == old_buf1.allocator);
    assert(buf2.len == old_buf2.len);
    assert(buf2.capacity == old_buf2.capacity);
    assert(buf2.buffer == old_buf2.buffer);
    assert(buf2.allocator == old_buf2.allocator);

    /* 6. Validity invariants must still hold */
    assert(aws_byte_buf_is_valid(&buf1));
    assert(aws_byte_buf_is_valid(&buf2));
}
