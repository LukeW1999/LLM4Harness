#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    /* Bound the list to keep verification tractable */
    struct aws_array_list list;

    /* Use nondet values for bounds */
    size_t max_items;
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(max_items <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize the list with known valid state */
    list.item_size = item_size;
    list.alloc = nondet_bool() ? NULL : can_fail_allocator();

    /* Set up length and current_size consistently */
    size_t length;
    size_t current_size;
    __CPROVER_assume(length <= max_items);
    __CPROVER_assume(current_size >= length * item_size);
    /* Avoid overflow */
    __CPROVER_assume(length < SIZE_MAX);
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);

    list.length = length;
    list.current_size = current_size;

    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Create a valid val pointer of item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    int result = aws_array_list_push_front(&list, val);

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Frame conditions: item_size and alloc never change */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* length increased by 1 */
        assert(list.length == old_length + 1);
        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
        /* data must be non-null */
        assert(list.data != NULL);
        /* first item_size bytes of data match val */
        assert(memcmp(list.data, val, list.item_size) == 0);
    } else {
        /* On failure, length and current_size are unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }
}
