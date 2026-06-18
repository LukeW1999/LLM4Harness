#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* allocator – use the default allocator as required by the harness rules */
    struct aws_allocator *alloc = aws_default_allocator();

    /* -----------------------------------------------------------------
     * 1. Declare and bound the source byte buffer (input)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    src.capacity = nondet_size_t();
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= src.capacity);
    src.allocator = alloc;

    if (src.capacity > 0) {
        ensure_byte_buf_has_allocated_buffer_member(&src);
    } else {
        src.buffer = NULL;
    }

    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;

    /* -----------------------------------------------------------------
     * 2. Declare the destination byte buffer (output)
     * ----------------------------------------------------------------- */
    struct aws_byte_buf dest;

    /* -----------------------------------------------------------------
     * 3. Call the function under verification
     * ----------------------------------------------------------------- */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* -----------------------------------------------------------------
     * 4. Post‑condition checks for both success and error paths
     * ----------------------------------------------------------------- */
    if (result == AWS_OP_SUCCESS) {
        /* src must remain unchanged */
        __CPROVER_assert(src.buffer == old_src.buffer, "src.buffer unchanged");
        __CPROVER_assert(src.len == old_src.len, "src.len unchanged");
        __CPROVER_assert(src.capacity == old_src.capacity, "src.capacity unchanged");
        __CPROVER_assert(src.allocator == old_src.allocator, "src.allocator unchanged");

        if (src.buffer == NULL) {
            /* src was an empty buffer – dest should be an empty buffer with the same allocator */
            __CPROVER_assert(dest.buffer == NULL, "dest.buffer NULL");
            __CPROVER_assert(dest.len == 0, "dest.len zero");
            __CPROVER_assert(dest.capacity == 0, "dest.capacity zero");
            __CPROVER_assert(dest.allocator == alloc, "dest.allocator matches");
        } else {
            /* src had data – dest must contain a copy of that data */
            __CPROVER_assert(dest.buffer != NULL, "dest.buffer non‑NULL");
            __CPROVER_assert(dest.buffer != src.buffer, "dest.buffer distinct");
            __CPROVER_assert(dest.len == src.len, "dest.len matches src.len");
            __CPROVER_assert(dest.capacity == src.capacity, "dest.capacity matches src.capacity");
            __CPROVER_assert(dest.allocator == alloc, "dest.allocator matches");
            /* the copied bytes must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Allocation failed – dest must be zeroed out */
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer NULL on failure");
        __CPROVER_assert(dest.len == 0, "dest.len zero on failure");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity zero on failure");
        __CPROVER_assert(dest.allocator == NULL, "dest.allocator NULL on failure");
    }

    /* -----------------------------------------------------------------
     * 5. Global invariants – the structures must remain valid
     * ----------------------------------------------------------------- */
    __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&src), "src valid");
}
