#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_array_eq_harness(void) {
    /* 1. nondet lengths, bounded */
    size_t len_a = nondet_size_t();
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    size_t len_b = nondet_size_t();
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);

    /* 2. allocate buffers according to lengths */
    uint8_t *buf_a = NULL;
    const void *array_a = NULL;
    if (len_a > 0) {
        buf_a = malloc(len_a);
        __CPROVER_assume(buf_a != NULL);
        /* fill with nondet data – optional, but makes the model nondet */
        for (size_t i = 0; i < len_a; ++i) {
            buf_a[i] = nondet_uint8_t();
        }
        array_a = buf_a;
    } else {
        array_a = NULL;
    }

    uint8_t *buf_b = NULL;
    const void *array_b = NULL;
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

    /* 3. Save old contents for immutability checks */
    struct store_byte_from_buffer storage_a;
    if (len_a > 0) {
        save_byte_from_array(buf_a, len_a, &storage_a);
    }
    struct store_byte_from_buffer storage_b;
    if (len_b > 0) {
        save_byte_from_array(buf_b, len_b, &storage_b);
    }

    /* 4. Call function under test */
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    /* 5. Assert inputs unchanged */
    if (len_a > 0) {
        assert_byte_from_buffer_matches(buf_a, &storage_a);
    }
    if (len_b > 0) {
        assert_byte_from_buffer_matches(buf_b, &storage_b);
    }

    /* 6. Assert post‑conditions derived from the specification */
    if (len_a != len_b) {
        /* lengths differ → must be false */
        assert(result == false);
    } else {
        /* lengths equal */
        if (len_a == 0) {
            /* both empty → true */
            assert(result == true);
        } else {
            /* non‑empty, compare memory */
            bool mem_eq = (memcmp(buf_a, buf_b, len_a) == 0);
            assert(result == mem_eq);
        }
    }

    /* 7. Clean up (not required for CBMC but keeps the harness tidy) */
    free(buf_a);
    free(buf_b);
}
