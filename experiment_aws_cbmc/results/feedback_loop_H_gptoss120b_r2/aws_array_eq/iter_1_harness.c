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
    /* nondeterministic lengths, bounded */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* nondeterministic buffers */
    void *array_a = NULL;
    void *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    /* Save old contents for immutability checks */
    struct store_byte_from_buffer old_a;
    struct store_byte_from_buffer old_b;

    if (len_a > 0) {
        save_byte_from_array((uint8_t *)array_a, len_a, &old_a);
    }
    if (len_b > 0) {
        save_byte_from_array((uint8_t *)array_b, len_b, &old_b);
    }

    /* Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* Postcondition: result matches specification */
    if (len_a != len_b) {
        assert(result == false);
    } else {
        if (len_a == 0) {
            assert(result == true);
        } else {
            assert(result == (!memcmp(array_a, array_b, len_a)));
        }
    }

    /* Postcondition: input arrays are unchanged */
    if (len_a > 0) {
        assert_byte_from_buffer_matches((uint8_t *)array_a, &old_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches((uint8_t *)array_b, &old_b);
    }

    /* Clean up */
    free(array_a);
    free(array_b);
}
