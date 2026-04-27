#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_array_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    const size_t array_len = nondet_size_t();
    const uint8_t *array = can_fail_malloc(array_len * sizeof(uint8_t));
    __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    const char *c_str = can_fail_malloc(MAX_BUFFER_SIZE * sizeof(char));
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    const uint8_t *old_array = array;
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* Success path: array and c_str must be equal up to array_len and c_str must be null-terminated */
        for (size_t i = 0; i < array_len; ++i) {
            assert(array[i] == (uint8_t)c_str[i]);
        }
        assert(c_str[array_len] == '\0');
    } else {
        /* Failure path: either array and c_str differ at some point or c_str is not null-terminated within array_len */
        bool arrays_differ = false;
        bool c_str_not_null_terminated = false;
        for (size_t i = 0; i < array_len; ++i) {
            if (array[i] != (uint8_t)c_str[i]) {
                arrays_differ = true;
                break;
            }
            if (c_str[i] == '\0') {
                break;
            }
        }
        if (!arrays_differ) {
            c_str_not_null_terminated = (c_str[array_len] != '\0');
        }
        assert(arrays_differ || c_str_not_null_terminated);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(array == old_array);
    assert(c_str == old_c_str);

    /* 6. Assert validity invariants always holds */
    /* No specific validity invariants for array and c_str in this context */
}
