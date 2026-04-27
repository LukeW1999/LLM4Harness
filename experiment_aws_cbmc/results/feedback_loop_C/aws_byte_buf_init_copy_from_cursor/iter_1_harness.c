// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init_copy_from_cursor returns AWS_OP_SUCCESS (or the successful value):
//   - dest.buffer: CHANGES to a new allocated buffer containing the contents of src.ptr
//   - dest.len: CHANGES to src.len
//   - dest.capacity: CHANGES to src.len
//   - dest.allocator: CHANGES to allocator

// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init_copy_from_cursor returns AWS_OP_ERR (or fails):
//   - dest.buffer: UNCHANGED (remains NULL)
//   - dest.len: UNCHANGED (remains 0)
//   - dest.capacity: UNCHANGED (remains 0)
//   - dest.allocator: UNCHANGED (remains allocator)

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   dest (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure
//   allocator (struct aws_allocator, if any):
//     - alloc: UNCHANGED always
//     - free: UNCHANGED always
//     - realloc: UNCHANGED always
//     - user_data: UNCHANGED always
//   src (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&src): YES (must hold before call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator allocator;
    struct aws_byte_cursor src;

    // Initialize src with a valid aws_byte_cursor
    size_t src_len = nondet_size_t();
    __CPROVER_assume(src_len <= MAX_BUFFER_SIZE);
    uint8_t *src_ptr = malloc(src_len);
    __CPROVER_assume(src_ptr != NULL || src_len == 0);
    src.ptr = src_ptr;
    src.len = src_len;

    // Initialize allocator with a valid aws_allocator
    allocator.alloc = malloc;
    allocator.free = free;
    allocator.realloc = realloc;
    allocator.user_data = NULL;

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Call the function under test
    int result = aws_byte_buf_init_copy_from_cursor(&dest, &allocator, src);

    // Step 1: Success Path Assertions
    if (result == AWS_OP_SUCCESS) {
        assert(dest.buffer != NULL);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == &allocator);
        assert(aws_byte_buf_is_valid(&dest));
        assert_bytes_match(dest.buffer, src.ptr, src.len);
    }

    // Step 2: Failure Path Assertions
    if (result == AWS_OP_ERR) {
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(aws_byte_buf_is_valid(&dest));
    }

    // Step 3: Frame Conditions Assertions
    assert(src.ptr == src_ptr);
    assert(src.len == src_len);
    assert(allocator.alloc == malloc);
    assert(allocator.free == free);
    assert(allocator.realloc == realloc);
    assert(allocator.user_data == NULL);

    // Step 4: Validity Invariants Assertions
    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));

    // Free allocated memory
    if (src_ptr != NULL) {
        free(src_ptr);
    }
    if (dest.buffer != NULL) {
        free(dest.buffer);
    }
}
