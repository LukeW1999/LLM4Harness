#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_pop_front_n_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* list under test */
    struct aws_array_list list;
    /* nondet initialization of list fields that are not set by the
     * aws_array_list_is_bounded / aws_array_list_is_valid assumptions */
    list.alloc = alloc;
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* allocate raw storage for the list – size is bounded by the
     * MAX_INITIAL_ITEM_ALLOCATION constant */
    size_t raw_capacity = nondet_size_t();
    __CPROVER_assume(raw_capacity <= MAX_INITIAL_ITEM_ALLOCATION);
    list.data = raw_capacity == 0 ? NULL : malloc(raw_capacity * list.item_size);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* remember original state */
    size_t old_length      = list.length;
    size_t old_current_sz  = list.current_size;
    void  *old_data_ptr    = list.data;
    size_t old_item_size   = list.item_size;

    /* make a copy of the original data buffer (if any) */
    uint8_t *old_data_copy = NULL;
    if (old_data_ptr != NULL && old_length > 0) {
        old_data_copy = malloc(old_length * old_item_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old_data_ptr, old_length * old_item_size);
    }

    /* nondet n for pop_front_n */
    size_t n = nondet_size_t();

    /* call the function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* --------------------------------------------------------------------
     * Post‑condition checks
     * -------------------------------------------------------------------- */

    /* 1. Length invariant */
    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old_length - n);
    }

    /* 2. Structural fields that must stay unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data_ptr);
    assert(list.current_size == old_current_sz);

    /* 3. Data preservation for the remaining elements */
    if (list.length > 0) {
        assert(old_data_copy != NULL);
        assert(memcmp(list.data,
                      old_data_copy + n * old_item_size,
                      list.length * old_item_size) == 0);
    }

    /* 4. Frame condition for the bytes that were removed.
     *    In a debug build they are overwritten with AWS_ARRAY_LIST_DEBUG_FILL,
     *    otherwise they must remain unchanged. */
#ifdef DEBUG_BUILD
    if (old_length > list.length) {
        size_t removed_bytes = (old_length - list.length) * old_item_size;
        uint8_t *fill_start = (uint8_t *)list.data + list.length * old_item_size;
        for (size_t i = 0; i < removed_bytes; ++i) {
            assert(fill_start[i] == AWS_ARRAY_LIST_DEBUG_FILL);
        }
    }
#else
    if (old_length > list.length) {
        size_t removed_bytes = (old_length - list.length) * old_item_size;
        uint8_t *orig_start = old_data_copy + list.length * old_item_size;
        uint8_t *new_start  = (uint8_t *)list.data + list.length * old_item_size;
        assert(memcmp(new_start, orig_start, removed_bytes) == 0);
    }
#endif

    /* clean up */
    free(old_data_copy);
    free(list.data);
    return 0;
}
