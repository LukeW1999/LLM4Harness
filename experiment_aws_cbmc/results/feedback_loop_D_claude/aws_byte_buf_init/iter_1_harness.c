// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init returns AWS_OP_SUCCESS:
//   - buf->buffer: CHANGES to NULL (if capacity==0) or valid allocated pointer (if capacity>0)
//   - buf->len: CHANGES to 0
//   - buf->capacity: CHANGES to capacity (the input parameter)
//   - buf->allocator: CHANGES to allocator (the input parameter)
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init returns AWS_OP_ERR (capacity != 0 and aws_mem_acquire returns NULL):
//   - buf->buffer: CHANGES to NULL (AWS_ZERO_STRUCT zeroes everything)
//   - buf->len: CHANGES to 0 (AWS_ZERO_STRUCT)
//   - buf->capacity: CHANGES to 0 (AWS_ZERO_STRUCT)
//   - buf->allocator: CHANGES to NULL (AWS_ZERO_STRUCT)
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buf->buffer: CHANGED always (either set to NULL/allocated on success, or zeroed on failure)
//   - buf->len: CHANGED always (set to 0 on success, zeroed on failure)
//   - buf->capacity: CHANGED always (set to capacity on success, zeroed on failure)
//   - buf->allocator: CHANGED always (set to allocator on success, zeroed on failure)
// allocator (struct aws_allocator *):
//   - all fields: UNCHANGED always (allocator is only used, not modified)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES on success path (postcondition in function)
//   - On failure path: buf is zeroed, so len=0, capacity=0, buffer=NULL, allocator=NULL
//     aws_byte_buf_is_valid checks: buf!=NULL, len<=capacity (0<=0 ok), capacity==0 so buffer can be NULL => VALID

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_buf_init_harness(void) {
    /* Allocate and set up inputs non-deterministically */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    size_t capacity = nondet_size_t();

    /* Preconditions: buf and allocator must be non-NULL */
    __CPROVER_assume(buf != NULL);
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep proof tractable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        /* === SUCCESS PATH assertions === */
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
        /* Validity invariant on success */
        assert(aws_byte_buf_is_valid(buf));
    } else {
        /* === FAILURE PATH assertions === */
        assert(result == AWS_OP_ERR);
        /* capacity must have been non-zero for failure to occur */
        assert(capacity != 0);
        /* AWS_ZERO_STRUCT was applied */
        assert(buf->buffer == NULL);
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
        /* Validity invariant on failure: zeroed struct is valid */
        assert(aws_byte_buf_is_valid(buf));
    }
}
