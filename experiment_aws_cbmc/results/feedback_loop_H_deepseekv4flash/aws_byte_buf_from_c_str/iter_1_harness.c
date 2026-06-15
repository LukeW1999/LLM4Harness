#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_c_str_harness() {
    /* Bound the buffer size */
    __CPROVER_assume(MAX_BUFFER_SIZE > 0);
    char c_str[MAX_BUFFER_SIZE];
    size_t null_pos;
    __CPROVER_assume(null_pos < MAX_BUFFER_SIZE);

    /* Set all characters before null terminator to non-zero */
    for (size_t i = 0; i < null_pos; i++) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != 0);
    }
    c_str[null_pos] = '\0';

    /* Non‑deterministically decide whether to pass NULL or the string */
    const char *c_str_ptr;
    if (nondet_bool()) {
        c_str_ptr = c_str;
    } else {
        c_str_ptr = NULL;
    }

    /* Call the function */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str_ptr);

    /* Postconditions */
    if (c_str_ptr) {
        /* len equals position of null terminator (i.e., string length) */
        assert(buf.len == null_pos);
        assert(buf.capacity == null_pos);
        assert(buf.buffer == (uint8_t *)c_str_ptr);
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    }
    assert(buf.allocator == NULL);

    /* Validity invariant holds */
    assert(aws_byte_buf_is_valid(&buf));
}
