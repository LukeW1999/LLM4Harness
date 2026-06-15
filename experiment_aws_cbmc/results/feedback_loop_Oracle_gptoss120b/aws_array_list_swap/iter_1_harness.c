#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initialization parameters */
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* initialize a dynamic list */
    aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* nondet length within capacity */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= list.length);
    list.length = len;

    /* fill the allocated data with nondet bytes */
    if (list.data != NULL && list.item_size > 0 && list.length > 0) {
        uint8_t *bytes = (uint8_t *)list.data;
        size_t total = list.length * list.item_size;
        for (size_t i = 0; i < total; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* indices to swap */
    size_t index_a = nondet_size_t();
    size_t index_b = nondet_size_t();

    /* ground‑truth preconditions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(index_a < aws_array_list_length(&list));
    __CPROVER_assume(index_b < aws_array_list_length(&list));

    /* snapshot of the list state before the call */
    size_t old_current_size = list.current_size;
    size_t old_length        = list.length;
    size_t old_item_size    = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data_ptr = list.data;

    uint8_t *old_data = NULL;
    size_t data_bytes = old_length * old_item_size;
    if (data_bytes > 0) {
        old_data = malloc(data_bytes);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, data_bytes);
    }

    /* call the function under verification */
    aws_array_list_swap(&list, index_a, index_b);

    /* postconditions */

    /* 1. validity predicates */
    assert(aws_array_list_is_valid(&list));

    /* 2. length / capacity invariants */
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data_ptr);

    /* 3. frame conditions – only the two elements may have changed */
    if (data_bytes > 0) {
        uint8_t *new_bytes = (uint8_t *)list.data;
        for (size_t i = 0; i < old_length; ++i) {
            uint8_t *new_elem = new_bytes + i * old_item_size;
            uint8_t *old_elem = old_data + i * old_item_size;

            if (i == index_a) {
                uint8_t *old_b = old_data + index_b * old_item_size;
                for (size_t j = 0; j < old_item_size; ++j) {
                    assert(new_elem[j] == old_b[j]);
                }
            } else if (i == index_b) {
                uint8_t *old_a = old_data + index_a * old_item_size;
                for (size_t j = 0; j < old_item_size; ++j) {
                    assert(new_elem[j] == old_a[j]);
                }
            } else {
                for (size_t j = 0; j < old_item_size; ++j) {
                    assert(new_elem[j] == old_elem[j]);
                }
            }
        }
        free(old_data);
    }

    return 0;
}
