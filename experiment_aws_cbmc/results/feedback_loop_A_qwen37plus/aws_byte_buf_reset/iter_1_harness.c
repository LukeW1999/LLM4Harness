#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_byte_buf_init_from_file_harness() {
    struct aws_byte_buf out_buf = {0};
    struct aws_allocator *alloc = aws_default_allocator();
    
    char filename[256];
    __CPROVER_assume(__CPROVER_is_zero_string(filename));
    
    int result = aws_byte_buf_init_from_file(&out_buf, alloc, filename);
    
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&out_buf));
        if (out_buf.capacity > 0) {
            assert(out_buf.buffer != NULL);
        }
    } else {
        assert(aws_byte_buf_is_valid(&out_buf));
    }
}
