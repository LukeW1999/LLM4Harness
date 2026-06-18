/* CBMC harness for aws_array_list_push_front */

#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"

void aws_array_list_push_front_harness(void) {
    /*--- Nondeterministic list setup ---*/
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* allocator: dynamic (non‑NULL) or static (NULL) */
    list.alloc = nondet_bool() ? alloc : NULL;

    /* item size > 0 */
    list.item_size = (size_t)nondet_uint();
    __CPROVER_assume(list.item_size > 0);

    /* length */
    list.length = (size_t)nondet_uint();

    /* extra capacity (in item units) for dynamic mode */
    size_t extra_items = (size_t)nondet_uint();

    /* current_size must be a multiple of item_size */
    list.current_size = (list.length + extra_items) * list.item_size;

    size_t old_capacity = list.current_size / list.item_size;
    __CPROVER_assume(list.length <= old_capacity);

    /* Allocate data buffer according to mode */
    if (list.alloc != NULL) {
        /* dynamic mode: data may be NULL if capacity is zero */
        if (old_capacity > 0) {
            list.data = aws_mem_acquire(list.alloc, list.current_size);
            __CPROVER_assume(list.data != NULL);
        } else {
            list.data = NULL;
        }
    } else {
        /* static mode: extra_items must be zero and capacity > 0 */
        __CPROVER_assume(extra_items == 0);
        __CPROVER_assume(old_capacity > 0);
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
    }

    /*--- Preserve old state ---*/
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = NULL;
    if (list.data != NULL && old_length > 0) {
        old_data = malloc(old_length * list.item_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_length * list.item_size);
    }

    /*--- Nondeterministic value to push ---*/
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_is_fresh(val, list.item_size));

    /*--- Call function under test ---*/
    int err = aws_array_list_push_front(&list, val);

    /*--- Postcondition checks ---*/
    if (err == 0) {
        /* length increased by one */
        assert(list.length == old_length + 1);

        /* new first element equals pushed value */
        assert(memcmp(list.data, val, list.item_size) == 0);

        /* existing elements shifted right unchanged */
        if (old_data != NULL && old_length > 0) {
            assert(memcmp((uint8_t *)list.data + list.item_size,
                          old_data,
                          old_length * list.item_size) == 0);
        }

        /* capacity may have grown (dynamic) or stay the same (static) */
        size_t new_capacity = list.current_size / list.item_size;
        assert(new_capacity >= old_capacity);
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* on error length unchanged and capacity unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        size_t new_capacity = list.current_size / list.item_size;
        assert(new_capacity == old_capacity);

        /* data unchanged */
        if (old_data != NULL && list.data != NULL && old_length > 0) {
            assert(memcmp(list.data, old_data, old_length * list.item_size) == 0);
        }
    }

    /*--- Frame conditions ---*/
    if (list.alloc != NULL) {
        assert(list.alloc == alloc);
    }

    /*--- Cleanup ---*/
    if (list.alloc != NULL && list.data != NULL) {
        aws_mem_release(list.alloc, list.data);
    } else {
        free(list.data);
    }
    free(val);
    free(old_data);
}
