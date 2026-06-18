#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               32U

/* nondeterministic helpers */
size_t nondet_size_t(void);
int    nondet_int(void);

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;
    size_t init_items = nondet_size_t();
    size_t item_sz    = nondet_size_t();

    __CPROVER_assume(init_items <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    /* initialize a dynamic list */
    int init_res = aws_array_list_init_dynamic(&list,
                                               aws_default_allocator(),
                                               init_items,
                                               item_sz);
    __CPROVER_assume(init_res == 0);

    /* structural validity assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t len = aws_array_list_length(&list);
    __CPROVER_assume(len > 0); /* ensure there is at least one element */

    /* choose indices within bounds */
    size_t index_a = nondet_size_t();
    size_t index_b = nondet_size_t();
    __CPROVER_assume(index_a < len);
    __CPROVER_assume(index_b < len);

    /* capture pre‑state of the whole backing buffer */
    size_t cap          = aws_array_list_capacity(&list);
    size_t total_bytes  = cap * list.item_size;
    uint8_t *pre_data   = malloc(total_bytes);
    __CPROVER_assume(pre_data != NULL);
    memcpy(pre_data, list.data, total_bytes);

    /* capture the two elements that will be swapped */
    void *elem_a = NULL;
    void *elem_b = NULL;
    aws_array_list_get_at_ptr(&list, &elem_a, index_a);
    aws_array_list_get_at_ptr(&list, &elem_b, index_b);

    uint8_t *buf_a = malloc(list.item_size);
    uint8_t *buf_b = malloc(list.item_size);
    __CPROVER_assume(buf_a != NULL && buf_b != NULL);
    memcpy(buf_a, elem_a, list.item_size);
    memcpy(buf_b, elem_b, list.item_size);

    /* call the function under verification */
    aws_array_list_swap(&list, index_a, index_b);

    /* 1. validity predicates */
    assert(aws_array_list_is_valid(&list));

    /* 2. length / capacity invariants */
    assert(aws_array_list_length(&list) == len);
    assert(aws_array_list_capacity(&list) == cap);

    /* 3. frame conditions: unchanged elements */
    for (size_t i = 0; i < len; ++i) {
        if (i != index_a && i != index_b) {
            void *ptr = NULL;
            aws_array_list_get_at_ptr(&list, &ptr, i);
            uint8_t *pre_ptr = pre_data + i * list.item_size;
            assert(memcmp(ptr, pre_ptr, list.item_size) == 0);
        }
    }

    /* swapped elements are exchanged */
    void *new_a = NULL;
    void *new_b = NULL;
    aws_array_list_get_at_ptr(&list, &new_a, index_a);
    aws_array_list_get_at_ptr(&list, &new_b, index_b);
    assert(memcmp(new_a, buf_b, list.item_size) == 0);
    assert(memcmp(new_b, buf_a, list.item_size) == 0);

    /* clean up */
    free(pre_data);
    free(buf_a);
    free(buf_b);
    aws_array_list_clean_up(&list);
    return 0;
}
