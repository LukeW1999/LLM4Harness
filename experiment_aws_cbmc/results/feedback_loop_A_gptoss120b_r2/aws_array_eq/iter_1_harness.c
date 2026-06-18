#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_eq_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    uint8_t *array_a;
    uint8_t *array_b;

    if (len_a == 0) {
        array_a = NULL;
    } else {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b == 0) {
        array_b = NULL;
    } else {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* 2. Save old state of the buffers for immutability checks */
    struct store_byte_from_buffer storage_a, storage_b;
    if (len_a > 0) {
        save_byte_from_array(array_a, len_a, &storage_a);
    }
    if (len_b > 0) {
        save_byte_from_array(array_b, len_b, &storage_b);
    }

    /* 3. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 4. Assert postconditions for both success and failure paths */
    if (result) {
        /* Success: lengths must be equal and contents identical */
        assert(len_a == len_b);
        if (len_a > 0) {
            assert(memcmp(array_a, array_b, len_a) == 0);
        }
    } else {
        /* Failure: either lengths differ, or lengths equal >0 and contents differ */
        assert(!(len_a == len_b && (len_a == 0 || memcmp(array_a, array_b, len_a) == 0)));
    }

    /* 5. Assert that the input buffers were not modified */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(array_a, &storage_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(array_b, &storage_b);
    }
}
