#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 3. Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* 4. Save old state of source (must remain unchanged) */
    struct aws_byte_buf old_src = src;

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 6. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest must be valid after success");
        __CPROVER_assert(dest.allocator == alloc, "dest allocator must be the one passed in");

        if (src.buffer == NULL) {
            __CPROVER_assert(dest.buffer == NULL, "dest.buffer must be NULL when src.buffer is NULL");
            __CPROVER_assert(dest.len == 0, "dest.len must be 0 when src.buffer is NULL");
            __CPROVER_assert(dest.capacity == 0, "dest.capacity must be 0 when src.buffer is NULL");
        } else {
            __CPROVER_assert(dest.buffer != NULL, "dest.buffer must be non‑NULL when src.buffer is non‑NULL");
            __CPROVER_assert(dest.len == src.len, "dest.len must equal src.len");
            __CPROVER_assert(dest.capacity == src.capacity, "dest.capacity must equal src.capacity");
            /* Verify contents are identical */
            for (size_t i = 0; i < src.len; ++i) {
                __CPROVER_assert(dest.buffer[i] == src.buffer[i],
                                 "dest buffer contents must match src");
            }
            /* New allocation, so pointers must differ */
            __CPROVER_assert(dest.buffer != src.buffer, "dest.buffer must be a different allocation");
        }
    } else {
        __CPROVER_assert(result == AWS_OP_ERR, "result must be AWS_OP_ERR on failure");
        __CPROVER_assert(dest.buffer == NULL, "dest.buffer must be NULL on failure");
        __CPROVER_assert(dest.len == 0, "dest.len must be 0 on failure");
        __CPROVER_assert(dest.capacity == 0, "dest.capacity must be 0 on failure");
        __CPROVER_assert(dest.allocator == NULL, "dest.allocator must be NULL on failure");
        __CPROVER_assert(aws_byte_buf_is_valid(&dest), "dest must be a valid empty buffer on failure");
    }

    /* 7. Source buffer must be unchanged regardless of outcome */
    __CPROVER_assert(src.buffer == old_src.buffer, "src.buffer unchanged");
    __CPROVER_assert(src.len == old_src.len, "src.len unchanged");
    __CPROVER_assert(src.capacity == old_src.capacity, "src.capacity unchanged");
    __CPROVER_assert(src.allocator == old_src.allocator, "src.allocator unchanged");
    __CPROVER_assert(aws_byte_buf_is_valid(&src), "src remains valid");
}
