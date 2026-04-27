// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init returns AWS_OP_SUCCESS:
//   - buf->len: CHANGES to 0
//   - buf->capacity: CHANGES to the requested capacity
//   - buf->allocator: CHANGES to the provided allocator
//   - buf->buffer: CHANGES to NULL if capacity==0, else a valid allocated pointer
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init returns AWS_OP_ERR (capacity != 0 and allocation fails):
//   - buf->len: CHANGES to 0 (AWS_ZERO_STRUCT zeroes it)
//   - buf->capacity: CHANGES to 0 (AWS_ZERO_STRUCT zeroes it)
//   - buf->allocator: CHANGES to NULL (AWS_ZERO_STRUCT zeroes it)
//   - buf->buffer: CHANGES to NULL (AWS_ZERO_STRUCT zeroes it)
//
// === STEP 3: FRAME CONDITIONS ===
//   buf (struct aws_byte_buf):
//     - buf->len: CHANGED always (set to 0 on success, zeroed on failure)
//     - buf->capacity: CHANGED always (set to capacity on success, 0 on failure)
//     - buf->allocator: CHANGED always (set to allocator on success, NULL on failure)
//     - buf->buffer: CHANGED always (allocated or NULL on success, NULL on failure)
//   allocator (struct aws_allocator*):
//     - all fields: UNCHANGED always (we don't modify the allocator struct)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES on success path (postcondition asserted by function)
//   - On failure path: buf is zeroed, so len=0, capacity=0, buffer=NULL → valid per predicate

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_harness(void) {
    /* Non-deterministic inputs */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    struct aws_allocator *allocator = nondet_bool() ? aws_default_allocator() : can_fail_allocator();
    size_t capacity;

    /* Preconditions: buf and allocator must be non-NULL */
    __CPROVER_assume(buf != NULL);
    __CPROVER_assume(allocator != NULL);

    /* Bound capacity to keep proof tractable */
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Call the function under test */
    int result = aws_byte_buf_init(buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        /* Step 1: Success path assertions */
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
        /* Step 4: Validity invariant on success */
        assert(aws_byte_buf_is_valid(buf));
    } else {
        /* Step 2: Failure path assertions */
        assert(result == AWS_OP_ERR);
        /* AWS_ZERO_STRUCT was applied, so all fields are zeroed */
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->allocator == NULL);
        assert(buf->buffer == NULL);
        /* Validity: len=0 <= capacity=0, capacity==0 so buffer can be NULL */
        assert(aws_byte_buf_is_valid(buf));
        /* Failure only possible when capacity != 0 and allocation failed */
        assert(capacity != 0);
    }
}
