#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_from_file_harness() {
    struct aws_byte_buf out_buf;
    __CPROVER_assume(aws_byte_buf_is_valid(&out_buf));
    
    struct aws_allocator *alloc = aws_default_allocator();
    
    size_t filename_len = nondet_size_t();
    __CPROVER_assume(filename_len < 1024);
    char *filename = malloc(filename_len + 1);
    __CPROVER_assume(filename != NULL);
    filename[filename_len] = '\0';

    struct aws_byte_buf old_out_buf = out_buf;

    int result = aws_byte_buf_init_from_file(&out_buf, alloc, filename);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&out_buf));
        assert(out_buf.allocator == alloc);
        assert(out_buf.capacity >= out_buf.len);
        if (out_buf.len > 0) {
            assert(out_buf.buffer != NULL);
        }
    } else {
        assert(aws_byte_buf_is_valid(&out_buf));
        assert(out_buf.buffer == NULL);
        assert(out_buf.len == 0);
        assert(out_buf.capacity == 0);
        assert(out_buf.allocator == NULL);
    }

    assert(aws_byte_buf_is_valid(&out_buf));
}
