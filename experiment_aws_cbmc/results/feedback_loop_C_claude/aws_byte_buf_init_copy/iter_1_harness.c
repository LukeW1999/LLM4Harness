// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_SUCCESS:
//   Case A: src->buffer == NULL
//     - dest is zeroed (dest->len = 0, dest->buffer = NULL, dest->capacity = 0)
//     - dest->allocator = allocator
//   Case B: src->buffer != NULL
//     - dest->len = src->len
//     - dest->capacity = src->capacity
//     - dest->allocator = allocator
//     - dest->buffer = newly allocated buffer (not NULL)
//     - dest->buffer[0..src->len-1] matches src->buffer[0..src->len-1]
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init_copy returns AWS_OP_ERR:
//   (only happens when src->buffer != NULL and allocation fails)
//     - dest is zeroed: dest->len = 0, dest->buffer = NULL, dest->capacity = 0, dest->allocator = NULL
//
// === STEP 3: FRAME CONDITIONS ===
//   dest (struct aws_byte_buf):
//     - dest->len: CHANGED on success (set to src->len or 0), CHANGED on failure (set to 0)
//     - dest->buffer: CHANGED on success (new allocation or NULL), CHANGED on failure (NULL)
//     - dest->capacity: CHANGED on success (set to src->capacity or 0), CHANGED on failure (set to 0)
//     - dest->allocator: CHANGED on success (set to allocator), CHANGED on failure (set to NULL)
//   src (const struct aws_byte_buf):
//     - src->len: UNCHANGED always
//     - src->buffer: UNCHANGED always
//     - src->capacity: UNCHANGED always
//     - src->allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(dest): YES (must hold after successful call)
//   - aws_byte_buf_is_valid(src): YES (precondition, must hold throughout)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness(void) {
    /* Allocate and set up src */
    struct aws_byte_buf *src = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(src != NULL);

    /* Make src a valid aws_byte_buf with bounded size */
    src->len = nondet_size_t();
    src->capacity = nondet_size_t();
    src->allocator = nondet_bool() ? NULL : can_fail_allocator();

    /* Ensure len <= capacity for validity */
    __CPROVER_assume(src->len <= src->capacity);

    /* Nondeterministically decide if buffer is NULL or allocated */
    if (nondet_bool()) {
        src->buffer = NULL;
        /* If buffer is NULL, len and capacity should be 0 for validity */
        src->len = 0;
        src->capacity = 0;
    } else {
        /* Bound the capacity to keep verification tractable */
        __CPROVER_assume(src->capacity <= MAX_BUFFER_SIZE);
        __CPROVER_assume(src->len <= src->capacity);
        src->buffer = malloc(src->capacity);
        __CPROVER_assume(src->buffer != NULL);
    }

    /* Precondition: src must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(src));

    /* Save old src state to verify frame conditions */
    struct aws_byte_buf old_src = *src;

    /* Set up dest (uninitialized, as it's an output parameter) */
    struct aws_byte_buf *dest = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(dest != NULL);

    /* Set up allocator */
    struct aws_allocator *allocator = can_fail_allocator();

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(dest, allocator, src);

    /* === Verify frame conditions for src (must be unchanged) === */
    assert(src->len == old_src.len);
    assert(src->buffer == old_src.buffer);
    assert(src->capacity == old_src.capacity);
    assert(src->allocator == old_src.allocator);

    /* === Verify postconditions based on result === */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid after successful call */
        assert(aws_byte_buf_is_valid(dest));

        /* allocator must be set correctly */
        assert(dest->allocator == allocator);

        if (old_src.buffer == NULL) {
            /* Case A: src->buffer was NULL */
            assert(dest->buffer == NULL);
            assert(dest->len == 0);
            assert(dest->capacity == 0);
        } else {
            /* Case B: src->buffer was not NULL */
            assert(dest->buffer != NULL);
            assert(dest->len == old_src.len);
            assert(dest->capacity == old_src.capacity);
            /* Verify the copy: contents match */
            if (old_src.len > 0) {
                assert_bytes_match(dest->buffer, old_src.buffer, old_src.len);
            }
        }
    } else {
        /* Failure path: dest should be zeroed */
        assert(result == AWS_OP_ERR);
        assert(dest->buffer == NULL);
        assert(dest->len == 0);
        assert(dest->capacity == 0);
        assert(dest->allocator == NULL);
    }
}
