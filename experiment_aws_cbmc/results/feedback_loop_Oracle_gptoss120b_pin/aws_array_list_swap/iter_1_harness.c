#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 8U
#define MAX_ITEM_SIZE               32U

void aws_array_list_swap_harness(void) {
    /* list allocation and initialization */
    struct aws_array_list list;
    size_t init_items = nondet_uint();
    __CPROVER_assume(init_items <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz = nondet_uint();
    __CPROVER_assume(item_sz > 0);
    __CPROVER_assume(item_sz <= MAX_ITEM_SIZE);

    aws_array_list_init_dynamic(&list, aws_default_allocator(), init_items, item_sz);

    /* indices */
    size_t index_a = nondet_uint();
    size_t index_b = nondet_uint();

    /* structural validity assumptions (as required) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(index_a < aws_array_list_length(&list));
    __CPROVER_assume(index_b < aws_array_list_length(&list));

    /* capture pre‑state */
    size_t old_length   = list.length;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_current  = list.current_size;
    void *old_data_ptr  = list.data;
    size_t old_item_sz  = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    unsigned char *old_bytes = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_bytes = malloc(list.current_size);
        __CPROVER_assume(old_bytes != NULL);
        memcpy(old_bytes, list.data, list.current_size);
    }

    /* call the function under verification */
    aws_array_list_swap(&list, index_a, index_b);

    /* 1. validity predicates */
    __CPROVER_assert(aws_array_list_is_valid(&list),
                     "list must remain valid after swap");

    /* 2. length / capacity invariants */
    __CPROVER_assert(list.length == old_length,
                     "list length must be unchanged after swap");
    __CPROVER_assert(aws_array_list_capacity(&list) == old_capacity,
                     "list capacity must be unchanged after swap");
    __CPROVER_assert(list.current_size == old_current,
                     "list current_size must be unchanged after swap");
    __CPROVER_assert(list.item_size == old_item_sz,
                     "list item_size must be unchanged after swap");
    __CPROVER_assert(list.alloc == old_alloc,
                     "list allocator must be unchanged after swap");
    __CPROVER_assert(list.data == old_data_ptr,
                     "list data pointer must be unchanged after swap");

    /* 3. frame conditions (memory unchanged except swapped elements) */
    if (old_bytes != NULL) {
        for (size_t i = 0; i < list.length; ++i) {
            unsigned char *elem_ptr = (unsigned char *)list.data + i * list.item_size;
            unsigned char *old_elem_ptr = old_bytes + i * list.item_size;

            if (i == index_a) {
                /* element at a must now equal old element at b */
                unsigned char *old_b_ptr = old_bytes + index_b * list.item_size;
                __CPROVER_assert(
                    memcmp(elem_ptr, old_b_ptr, list.item_size) == 0,
                    "element at index_a must be swapped with element at index_b");
            } else if (i == index_b) {
                /* element at b must now equal old element at a */
                unsigned char *old_a_ptr = old_bytes + index_a * list.item_size;
                __CPROVER_assert(
                    memcmp(elem_ptr, old_a_ptr, list.item_size) == 0,
                    "element at index_b must be swapped with element at index_a");
            } else {
                /* all other elements must be unchanged */
                __CPROVER_assert(
                    memcmp(elem_ptr, old_elem_ptr, list.item_size) == 0,
                    "non‑swapped elements must remain unchanged");
            }
        }
        free(old_bytes);
    }

    return 0;
}
