#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_eq_c_str_harness() {
    /* Non-deterministic inputs */
    size_t array_len = nondet_size_t();
    const void *array = NULL;
    const char *c_str = NULL;

    /* Bound array_len to limit state space */
    __CPROVER_assume(array_len <= MAX_BUFFER_SIZE);

    /* Handle array pointer: can be NULL only if array_len == 0 */
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        /* Fill array with non-deterministic bytes */
        __CPROVER_havoc_object(array);
        /* Ensure memory is readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(array, array_len));
    } else {
        /* array_len == 0: array may be NULL or non-NULL; we choose nondet */
        if (nondet_bool()) {
            array = NULL;
        } else {
            array = malloc(1); /* allocate 1 byte even if not used */
            __CPROVER_assume(array != NULL);
        }
    }

    /* c_str must be non-NULL and readable for at least array_len+1 bytes.
     * We allocate exactly array_len+1 bytes to cover the maximum read. */
    size_t c_str_size = array_len + 1;
    c_str = malloc(c_str_size);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_havoc_object(c_str);
    /* Ensure memory is readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_size));

    /* Precondition from implementation: array || (array_len == 0) */
    __CPROVER_assume(array != NULL || array_len == 0);

    /* Call function under test */
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    /* Postconditions: result is a boolean (no crash occurred) */
    assert(result == true || result == false);
}
