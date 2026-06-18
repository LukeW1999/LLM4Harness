#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initial allocation and item size within the allowed bounds */
    size_t init_alloc = nondet_size_t();
    __CPROVER_assume(init_alloc <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0U && item_sz <= MAX_ITEM_SIZE);

    /* initialize the list in dynamic mode */
    if (aws_array_list_init_dynamic(&list, alloc, init_alloc, item_sz) != 0) {
        return 0;
    }

    /* allocate a nondet value buffer of the appropriate size */
    uint8_t *val = malloc(item_sz);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0U; i < item_sz; ++i) {
        val[i] = nondet_uint8_t();
    }

    /* snapshot old state of the list */
    size_t old_len = aws_array_list_length(&list);
    size_t old_cap = aws_array_list_capacity(&list);
    uint8_t *old_data = NULL;
    if (list.data != NULL && old_cap > 0U) {
        old_data = malloc(old_cap * item_sz);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_cap * item_sz);
    }

    /* ground‑truth preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(val && AWS_MEM_IS_READABLE(val, list.item_size));

    /* call the function under test */
    int ret = aws_array_list_push_front(&list, val);

    /* post‑conditions */

    /* the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    if (ret == 0) {
        /* length increased by one */
        assert(aws_array_list_length(&list) == old_len + 1U);
        /* capacity never shrinks */
        assert(aws_array_list_capacity(&list) >= old_cap);
        /* the new front element matches the supplied value */
        uint8_t *front = (uint8_t *)list.data;
        for (size_t i = 0U; i < list.item_size; ++i) {
            assert(front[i] == val[i]);
        }
        /* existing elements are shifted correctly */
        if (old_len > 0U) {
            uint8_t *shifted = (uint8_t *)list.data + list.item_size;
            for (size_t i = 0U; i < old_len * list.item_size; ++i) {
                assert(shifted[i] == old_data[i]);
            }
        }
    } else {
        /* on error the length is unchanged */
        assert(aws_array_list_length(&list) == old_len);
        /* the underlying buffer is unchanged */
        if (old_cap > 0U && list.data != NULL) {
            for (size_t i = 0U; i < old_cap * list.item_size; ++i) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    /* clean up */
    if (old_data) {
        free(old_data);
    }
    free(val);
    aws_array_list_clean_up(&list);
    return 0;
}
