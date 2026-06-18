#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet initial allocation and item size within bounds */
    size_t init_items;
    __CPROVER_assume(init_items <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t item_sz;
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);

    /* initialize a dynamic array list */
    aws_array_list_init_dynamic(&list, alloc, init_items, item_sz);

    /* nondet length (number of elements) respecting capacity */
    size_t len;
    __CPROVER_assume(len <= init_items);
    list.length = len;
    list.current_size = len * item_sz;

    /* structural validity assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* capture pre‑state */
    size_t old_len          = list.length;
    size_t old_current_size = list.current_size;
    size_t old_capacity     = aws_array_list_capacity(&list);
    void  *old_data         = list.data;

    /* call the function under verification */
    int ret = aws_array_list_pop_front(&list);

    /* post‑conditions */
    if (old_len > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(list.length == old_len - 1);
        assert(list.current_size == (old_len - 1) * list.item_size);
    } else {
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(list.length == old_len);
        assert(list.current_size == old_current_size);
    }

    /* capacity and data pointer must remain unchanged */
    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(list.data == old_data);

    /* the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    return 0;
}
