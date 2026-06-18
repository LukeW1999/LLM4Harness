#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness(void) {
    /* Allocate and initialize a byte buffer */
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    uint8_t *data = NULL;
    if (capacity > 0) {
        data = malloc(capacity);
        __CPROVER_assume(data != NULL);
        /* make the allocated memory fresh for CBMC */
        __CPROVER_assume(__CPROVER_is_fresh(data, capacity));
    }

    buf.buffer    = data;
    buf.capacity  = capacity;
    buf.len       = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = alloc;

    /* Structural validity assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Snapshot unrelated memory for frame condition */
    int dummy = nondet_int();
    int dummy_old = dummy;

    /* Call the function under verification */
    aws_byte_buf_clean_up_secure(&buf);

    /* Post‑condition checks */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Frame condition: unrelated memory must be unchanged */
    assert(dummy == dummy_old);

    return 0;
}
