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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_eq_harness(void) {
    /* 1. nondeterministic lengths bounded by MAX_BUFFER_SIZE */
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. allocate buffers if length > 0, otherwise set to NULL */
    const void *array_a;
    const void *array_b;
    uint8_t *buf_a = NULL;
    uint8_t *buf_b = NULL;

    if (len_a > 0) {
        buf_a = malloc(len_a);
        __CPROVER_assume(buf_a != NULL);
        for (size_t i = 0; i < len_a; ++i) {
            buf_a[i] = nondet_uint8_t();
        }
        array_a = buf_a;
    } else {
        array_a = NULL;
    }

    if (len_b > 0) {
        buf_b = malloc(len_b);
        __CPROVER_assume(buf_b != NULL);
        for (size_t i = 0; i < len_b; ++i) {
            buf_b[i] = nondet_uint8_t();
        }
        array_b = buf_b;
    } else {
        array_b = NULL;
    }

    /* 3. snapshot old state for immutability checks */
    const void *old_array_a = array_a;
    const void *old_array_b = array_b;
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    uint8_t *old_contents_a = NULL;
    uint8_t *old_contents_b = NULL;
    if (len_a > 0) {
        old_contents_a = malloc(len_a);
        __CPROVER_assume(old_contents_a != NULL);
        memcpy(old_contents_a, buf_a, len_a);
    }
    if (len_b > 0) {
        old_contents_b = malloc(len_b);
        __CPROVER_assume(old_contents_b != NULL);
        memcpy(old_contents_b, buf_b, len_b);
    }

    /* 4. call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. postconditions on return value */
    if (len_a != len_b) {
        assert(result == false);
    } else {
        if (len_a == 0) {
            assert(result == true);
        } else {
            assert(result == (memcmp(array_a, array_b, len_a) == 0));
        }
    }

    /* 6. unchanged inputs (frame conditions) */
    assert(array_a == old_array_a);
    assert(array_b == old_array_b);
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);

    if (len_a > 0) {
        assert(memcmp(buf_a, old_contents_a, len_a) == 0);
    } else {
        assert(array_a == NULL);
    }
    if (len_b > 0) {
        assert(memcmp(buf_b, old_contents_b, len_b) == 0);
    } else {
        assert(array_b == NULL);
    }

    /* 7. clean up */
    free(buf_a);
    free(buf_b);
    free(old_contents_a);
    free(old_contents_b);
}
