#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
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
    list.alloc = alloc;

    /* nondet item size */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* nondet raw capacity (in items) */
    size_t raw_capacity = nondet_size_t();
    __CPROVER_assume(raw_capacity <= MAX_INITIAL_ITEM_ALLOCATION);

    /* allocate storage */
    list.data = raw_capacity == 0 ? NULL : malloc(raw_capacity * list.item_size);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));

    /* nondet length, must fit in allocated capacity */
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= raw_capacity);
    if (raw_capacity == 0) {
        __CPROVER_assume(list.length == 0);
    }

    /* current_size reflects allocated bytes */
    list.current_size = raw_capacity * list.item_size;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* remember original state */
    size_t old_length      = list.length;
    size_t old_current_sz  = list.current_size;
    void  *old_data_ptr    = list.data;
    size_t old_item_size   = list.item_size;

    /* copy original data buffer */
    uint8_t *old_data_copy = NULL;
    if (old_data_ptr != NULL && old_length > 0) {
        old_data_copy = malloc(old_length * old_item_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, old_data_ptr, old_length * old_item_size);
    }

    /* nondet n for pop_front_n, must be <= old_length */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= old_length);

    /* call function under verification */
    aws_array_list_pop_front_n(&list, n);

    /* --------------------------------------------------------------------
     * Post‑condition checks
     * -------------------------------------------------------------------- */

    /* length invariant */
    assert(list.length == old_length - n);

    /* structural fields unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data_ptr);
    assert(list.current_size == old_current_sz);

    /* data preservation for remaining elements */
    if (list.length > 0) {
        assert(old_data_copy != NULL);
        assert(memcmp(list.data,
                      old_data_copy + n * old_item_size,
                      list.length * old_item_size) == 0);
    }

    /* clean up */
    free(old_data_copy);
    free(list.data);
}
