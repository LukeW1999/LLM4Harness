#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_eq_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t array_len = nondet_size_t();
    const char *c_str = can_fail_nondet_char_ptr();

    // Allocate memory for array using the allocator
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = (uint8_t *)malloc(array_len);
        assume(array != NULL); // Ensure allocation succeeds if size is non-zero
    }

    // Save old state
    uint8_t *old_array = array;
    size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    // Assume preconditions
    __CPROVER_assume(array || (array_len == 0));
    if (c_str) {
        size_t c_str_len = strlen(c_str);
        __CPROVER_assume(array_len <= c_str_len);
    } else {
        __CPROVER_assume(array_len == 0);
    }

    // Call the function under test
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // Assert frame conditions
    assert(array == old_array);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str);

    // Assert postconditions
    // Since the function is a comparison, no changes are expected in the parameters.

    // Free allocated memory
    free(array);
}
