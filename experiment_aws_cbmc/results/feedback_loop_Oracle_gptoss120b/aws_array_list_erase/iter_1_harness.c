#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

void aws_array_list_erase_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* list under test */
    struct aws_array_list list;
    /* nondet initialization of the list fields is performed inside the
       helper functions used by the assumptions below. */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* capture pre‑state */
    size_t old_length       = aws_array_list_length(&list);
    size_t old_current_size = list.current_size;
    size_t old_item_size    = list.item_size;
    uint8_t *old_data       = NULL;

    if (list.data != NULL && old_current_size > 0) {
        old_data = malloc(old_current_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_current_size);
    }

    /* nondet index */
    size_t index = nondet_size_t();

    /* call the function under verification */
    int ret = aws_array_list_erase(&list, index);

    /* 1. return value / error code correctness */
    if (index >= old_length) {
        /* out‑of‑bounds */
        assert(ret != AWS_OP_SUCCESS);
    } else {
        /* successful erase */
        assert(ret == AWS_OP_SUCCESS);
    }

    /* 2. length / capacity invariants */
    if (index >= old_length) {
        /* no change */
        assert(aws_array_list_length(&list) == old_length);
    } else {
        /* one element removed */
        assert(aws_array_list_length(&list) == old_length - 1);
    }
    /* capacity (current_size) must stay the same */
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == alloc);

    /* 3. frame condition – memory outside the modified region unchanged */
    if (old_data != NULL) {
        size_t unchanged_start;
        if (index >= old_length) {
            /* nothing was modified */
            unchanged_start = 0;
        } else {
            /* bytes after the original logical end are untouched */
            unchanged_start = old_length * old_item_size;
        }
        for (size_t i = unchanged_start; i < old_current_size; ++i) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }

    return 0;
}
