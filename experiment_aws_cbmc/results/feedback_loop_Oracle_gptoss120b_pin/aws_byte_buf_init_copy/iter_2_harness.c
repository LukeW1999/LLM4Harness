#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_init_copy_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* source buffer */
    struct aws_byte_buf src;
    src.allocator = allocator;

    /* nondet capacity bounded by MAX_BUFFER_SIZE */
    src.capacity = nondet_size_t();
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);

    /* nondet length not exceeding capacity */
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= src.capacity);

    /* allocate buffer if capacity > 0 */
    if (src.capacity > 0) {
        src.buffer = malloc(src.capacity);
        __CPROVER_assume(src.buffer != NULL);
        /* fill with nondet data */
        for (size_t i = 0; i < src.capacity; ++i) {
            src.buffer[i] = nondet_uint8_t();
        }
    } else {
        src.buffer = NULL;
    }

    /* destination buffer */
    struct aws_byte_buf dest;

    /* remember original src state */
    size_t src_len_orig = src.len;
    size_t src_cap_orig = src.capacity;
    uint8_t *src_buf_orig = src.buffer;
    uint8_t src_content[MAX_BUFFER_SIZE];
    if (src_buf_orig != NULL) {
        for (size_t i = 0; i < src_cap_orig; ++i) {
            src_content[i] = src_buf_orig[i];
        }
    }

    /* preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* call function under test */
    int ret = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* allocator must be set */
        assert(dest.allocator == allocator);

        /* length and capacity must match source */
        assert(dest.len == src_len_orig);
        assert(dest.capacity == src_cap_orig);

        if (src_buf_orig == NULL) {
            /* when source buffer is NULL, dest buffer must be NULL */
            assert(dest.buffer == NULL);
        } else {
            /* dest buffer must be non‑NULL and contain a copy of source data */
            assert(dest.buffer != NULL);
            for (size_t i = 0; i < src_len_orig; ++i) {
                assert(dest.buffer[i] == src_content[i]);
            }
        }
    } else {
        /* on error, dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* source must remain unchanged */
    assert(src.len == src_len_orig);
    assert(src.capacity == src_cap_orig);
    assert(src.buffer == src_buf_orig);
    if (src_buf_orig != NULL) {
        for (size_t i = 0; i < src_cap_orig; ++i) {
            assert(src.buffer[i] == src_content[i]);
        }
    }

    /* clean up */
    if (src.buffer) {
        free(src.buffer);
    }
    if (dest.buffer) {
        free(dest.buffer);
    }
}
