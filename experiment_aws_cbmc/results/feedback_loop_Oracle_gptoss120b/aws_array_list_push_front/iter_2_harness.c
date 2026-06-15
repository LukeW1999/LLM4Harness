#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;
    struct aws_array_list old_list;
    uint8_t *old_data = NULL;
    size_t old_capacity = 0;

    /* nondet item size within bounds */
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* nondet initial allocation within bounds */
    size_t init_alloc = nondet_size_t();
    __CPROVER_assume(init_alloc <= MAX_INITIAL_ITEM_ALLOCATION);

    /* initialize list in dynamic mode with the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    aws_array_list_init_dynamic(&list, allocator, init_alloc, item_size);

    /* nondet length that respects current capacity */
    size_t capacity = aws_array_list_capacity(&list);
    size_t orig_len = nondet_size_t();
    __CPROVER_assume(orig_len <= capacity);
    list.length = orig_len;

    /* allocate and nondet‑initialize the backing storage up to capacity */
    if (capacity > 0) {
        list.data = malloc(capacity * item_size);
        __CPROVER_assume(list.data != NULL);
        uint8_t *d = (uint8_t *)list.data;
        for (size_t i = 0; i < capacity * item_size; ++i) {
            d[i] = nondet_uint8_t();
        }
    } else {
        list.data = NULL;
    }

    /* allocate a nondet value to push */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; ++i) {
        ((uint8_t *)val)[i] = nondet_uint8_t();
    }

    /* apply the required preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot old state for post‑condition checks */
    old_list = list;
    old_capacity = aws_array_list_capacity(&old_list);
    if (old_capacity > 0) {
        old_data = malloc(old_capacity * item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_capacity * item_size);
    }

    /* call the function under verification */
    int ret = aws_array_list_push_front(&list, val);

    /* -------------------------------------------------------------------- */
    /* Post‑condition 1: return value / error code correctness               */
    /* -------------------------------------------------------------------- */
    assert(aws_array_list_is_valid(&list));

    /* -------------------------------------------------------------------- */
    /* Post‑condition 2: length / capacity invariants                        */
    /* -------------------------------------------------------------------- */
    if (ret == 0) {
        assert(list.length == old_list.length + 1);
        assert(aws_array_list_capacity(&list) >= old_capacity);
    } else {
        assert(list.length == old_list.length);
        assert(aws_array_list_capacity(&list) == old_capacity);
    }

    /* -------------------------------------------------------------------- */
    /* Post‑condition 3: frame conditions (memory not modified beyond contract) */
    /* -------------------------------------------------------------------- */
    if (old_capacity > 0) {
        uint8_t *new_data = (uint8_t *)list.data;
        if (ret == 0) {
            /* newly inserted element must equal the input value */
            assert(memcmp(new_data, val, item_size) == 0);
            /* existing elements must be shifted right by one position */
            for (size_t i = 0; i < old_list.length; ++i) {
                assert(memcmp(new_data + (i + 1) * item_size,
                              old_data + i * item_size,
                              item_size) == 0);
            }
        } else {
            /* on error the data buffer must be unchanged */
            assert(memcmp(new_data, old_data, old_capacity * item_size) == 0);
        }
    }

    /* clean up */
    free(old_data);
    free(list.data);
    free(val);
}
