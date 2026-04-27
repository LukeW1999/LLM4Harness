#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_eq_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    const void *array = nd_void_ptr();
    size_t array_len = nd_size_t();
    const char *c_str = nd_char_ptr();

    // Save old state
    const void *old_array = array;
    size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // Frame conditions
    assert(array == old_array);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str);

    // Validity invariants
    // No validity invariants to check as the function does not modify the inputs
}
