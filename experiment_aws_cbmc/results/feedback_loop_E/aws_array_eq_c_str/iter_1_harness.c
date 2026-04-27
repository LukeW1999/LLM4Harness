#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

void aws_array_eq_c_str_harness() {
    /* 1. Declare and bound data structures */
    const void *array = (const void *)nondet_uint8_t_ptr();
    size_t array_len = nondet_size_t();
    const char *c_str = (const char *)nondet_uint8_t_ptr();

    /* Bounding assumptions */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, array_len + 1)); /* +1 for null terminator */

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    /* Not needed for this function as it is a pure function with no side effects */

    /* 3. Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        /* On success, array and c_str must be equivalent up to array_len and c_str must be null-terminated */
        for (size_t i = 0; i < array_len; ++i) {
            assert(((const uint8_t *)array)[i] == ((const uint8_t *)c_str)[i]);
        }
        assert(((const uint8_t *)c_str)[array_len] == '\0');
    } else {
        /* On failure, either array and c_str differ at some point or c_str is not null-terminated */
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
    /* No fields to assert as this is a pure function */

    /* 6. Assert validity invariant always holds */
    /* No validity invariants to assert as this is a pure function */
}
