#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>

void aws_array_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    const void *array = bounded_malloc(MAX_BUFFER_SIZE);
    size_t array_len = nondet_size_t();
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    const char *c_str = bounded_malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, array_len + 1)); // +1 for null terminator

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    // No state to save for this function

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    // No specific fields change, so we only need to check the result and validity
    if (result) {
        // Check that array and c_str are equal up to array_len and c_str is null-terminated after array_len
        for (size_t i = 0; i < array_len; ++i) {
            assert(((const uint8_t *)array)[i] == ((const uint8_t *)c_str)[i]);
        }
        assert(((const uint8_t *)c_str)[array_len] == '\0');
    } else {
        // Either array and c_str differ at some point, or c_str is not null-terminated after array_len
        bool arrays_differ = false;
        for (size_t i = 0; i < array_len; ++i) {
            if (((const uint8_t *)array)[i] != ((const uint8_t *)c_str)[i]) {
                arrays_differ = true;
                break;
            }
        }
        bool c_str_not_null_terminated = ((const uint8_t *)c_str)[array_len] != '\0';
        assert(arrays_differ || c_str_not_null_terminated);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No fields to assert for this function as it does not modify any input parameters

    /* 6. Assert validity invariants always holds */
    // No specific validity invariants for this function as it does not modify any input parameters
}
