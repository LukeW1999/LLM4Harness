#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t initial_item_count = nondet_size_t();
    __CPROVER_assume(initial_item_count > 0);
    __CPROVER_assume(initial_item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Use dynamic list only to avoid complexity with static buffer sizing */
    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_count, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Optionally pre-populate some items */
    size_t existing_items = nondet_size_t();
    __CPROVER_assume(existing_items <= initial_item_count);
    list.length = existing_items;
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Create val buffer */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Assert result is valid */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: length increases by 1 */
        assert(list.length == old_length + 1);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
        /* data must be non-null */
        assert(list.data != NULL);
        /* current_size must accommodate the new length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure: length unchanged */
        assert(list.length == old_length);
        /* item_size unchanged */
        assert(list.item_size == old_item_size);
        /* alloc unchanged */
        assert(list.alloc == old_alloc);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
