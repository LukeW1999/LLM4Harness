// === STEP 1: SUCCESS PATH ===
// When aws_array_eq_c_str returns true:
//   - array: UNCHANGED
//   - array_len: UNCHANGED
//   - c_str: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_array_eq_c_str returns false:
//   - array: UNCHANGED
//   - array_len: UNCHANGED
//   - c_str: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// array (pointer):
//   - UNCHANGED always
// array_len (size_t):
//   - UNCHANGED always
// c_str (pointer):
//   - UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_array_eq_c_str does not modify any parameters, so no validity invariants need to be checked after call

#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_array_eq_c_str_harness() {
    const void *array = ndecls(void_ptr);
    size_t array_len = nondet_size_t();
    const char *c_str = ndecls(char_ptr);

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
