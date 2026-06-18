#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_harness() {
    /* 1. Declare and bound data structures */
    size_t len_a;
    size_t len_b;
    
    /* Bound the lengths to avoid state space explosion */
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
    
    /* Allocate buffers conditionally: if length > 0, buffer must be readable */
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;
    
    if (len_a > 0) {
        array_a = malloc(len_a);
        /* malloc can fail */
        __CPROVER_assume(array_a != NULL);
    }
    
    if (len_b > 0) {
        array_b = malloc(len_b);
        /* malloc can fail */
        __CPROVER_assume(array_b != NULL);
    }
    
    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct store_byte_from_buffer old_byte_from_array_a;
    struct store_byte_from_buffer old_byte_from_array_b;
    
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &old_byte_from_array_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &old_byte_from_array_b);
    }
    
    /* 3. Call function under test */
    bool result = aws_array_eq((const void *)array_a, len_a, (const void *)array_b, len_b);
    
    /* 4. Assert postconditions: function returns true iff contents are equivalent */
    if (len_a != len_b) {
        /* Different lengths → must return false */
        assert(result == false);
    } else if (len_a == 0) {
        /* Both zero length → must return true */
        assert(result == true);
    } else {
        /* Same non-zero length: result depends on memory content */
        /* If arrays are identical, result must be true */
        /* If arrays differ, result must be false */
        bool arrays_identical = (memcmp(array_a, array_b, len_a) == 0);
        if (arrays_identical) {
            assert(result == true);
        } else {
            assert(result == false);
        }
    }
    
    /* 5. Assert fields that must NOT change regardless of result */
    /* Arrays are read-only; verify buffer contents unchanged */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &old_byte_from_array_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &old_byte_from_array_b);
    }
    
    /* 6. Free allocated memory */
    if (len_a > 0) {
        free(array_a);
    }
    if (len_b > 0) {
        free(array_b);
    }
}
