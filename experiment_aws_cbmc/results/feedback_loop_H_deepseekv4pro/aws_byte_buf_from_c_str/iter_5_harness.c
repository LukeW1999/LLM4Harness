#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_from_c_str_harness() {
    const char *c_str;
    size_t len;
    bool is_null = nondet_bool();
    struct store_byte_from_buffer storage;
    struct aws_allocator *allocator = aws_default_allocator();

    if (is_null) {
        c_str = NULL;
    } else {
        /* allocate a non-deterministic string with a max length */
        len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *str_buf = (char *)aws_mem_acquire(allocator, len + 1);
        if (str_buf == NULL) {
            /* allocation failure – skip this test run */
            return;
        }
        /* fill with arbitrary non-zero bytes, no embedded nulls */
        for (size_t i = 0; i < len; i++) {
            char c = (char)nondet_uint8_t();
            __CPROVER_assume(c != '\0');
            str_buf[i] = c;
        }
        /* null-terminate */
        str_buf[len] = '\0';
        c_str = str_buf;

        /* save the initial state of the string for immutability check */
        save_byte_from_array((const uint8_t *)c_str, len + 1, &storage);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* Assume that the internal allocation in aws_byte_buf_from_c_str succeeds
       when the input string is non-NULL. This restricts verification to
       the successful path, which is the intended use of the function. */
    if (c_str != NULL) {
        __CPROVER_assume(buf.buffer != NULL);
    }

    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.buffer != NULL);
        assert(buf.len == len);
        assert(buf.capacity == len);
        assert(buf.allocator == NULL);
        /* verify that the buffer contents match the original string */
        assert_bytes_match(buf.buffer, (const uint8_t *)c_str, buf.len);
        /* verify that the original string was not modified */
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &storage);
    }

    assert(aws_byte_buf_is_valid(&buf));

    /* free the copied buffer if allocated */
    if (buf.buffer != NULL) {
        aws_mem_release(allocator, buf.buffer);
    }

    /* free the original string memory */
    if (!is_null) {
        aws_mem_release(allocator, (void *)c_str);
    }
}
