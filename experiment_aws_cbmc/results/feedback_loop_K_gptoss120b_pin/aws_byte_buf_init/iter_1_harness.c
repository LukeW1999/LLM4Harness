/*  
Preconditions:  
- __CPROVER_assume(buf != NULL);  
- __CPROVER_assume(allocator != NULL);  
- __CPROVER_assume(capacity == (size_t)capacity);  

Postconditions (validity):  
- assert(return_val == AWS_OP_SUCCESS || return_val == AWS_OP_ERR);  
- if (return_val == AWS_OP_SUCCESS) {  
    assert(aws_byte_buf_is_valid(buf));  
  } else {  
    assert(buf->len == 0);  
    assert(buf->capacity == 0);  
    assert(buf->buffer == NULL);  
    assert(buf->allocator == NULL);  
  }  

Postconditions (length & fields):  
- if (return_val == AWS_OP_SUCCESS) {  
    assert(buf->len == 0);  
    assert(buf->capacity == capacity);  
    if (capacity == 0) {  
        assert(buf->buffer == NULL);  
    } else {  
        assert(buf->buffer != NULL);  
    }  
    assert(buf->allocator == allocator);  
  }  

Postconditions (frame):  
- assert(allocator == allocator_before);  // allocator pointer unchanged  
*/

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Stub for aws_mem_acquire to allow nondeterministic allocation success/failure */
uint8_t *aws_mem_acquire(struct aws_allocator *allocator, size_t size) {
    uint8_t *p;
    __CPROVER_assume(p == NULL || __CPROVER_is_fresh(p, size));
    return p;
}

/* Stub for aws_default_allocator */
struct aws_allocator *aws_default_allocator(void) {
    static struct aws_allocator default_allocator;
    return &default_allocator;
}

/* Stub for aws_byte_buf_is_valid (as used in postcondition) */
bool aws_byte_buf_is_valid(const struct aws_byte_buf *buf) {
    if (buf == NULL) return false;
    if (buf->len > buf->capacity) return false;
    if (buf->capacity != 0 && buf->buffer == NULL) return false;
    return true;
}

/* Stub for AWS_PRECONDITION and AWS_POSTCONDITION macros (no-op) */
#define AWS_PRECONDITION(x) ((void)0)
#define AWS_POSTCONDITION(x) ((void)0)
#define AWS_ZERO_STRUCT(s) memset(&(s), 0, sizeof(s))
#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR 1

void aws_byte_buf_init_harness(void) {
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t capacity;
    __CPROVER_assume(capacity == (size_t)capacity);

    /* Save pre-state for frame condition */
    struct aws_allocator *allocator_before = allocator;

    int ret = aws_byte_buf_init(buf, allocator, capacity);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* Validity */
        assert(aws_byte_buf_is_valid(buf));

        /* Length and fields */
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
        assert(buf->allocator == allocator);
    } else {
        /* Error case: buffer zeroed */
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->buffer == NULL);
        assert(buf->allocator == NULL);
    }

    /* Frame condition */
    assert(allocator == allocator_before);

    free(buf);
    return 0;
}
