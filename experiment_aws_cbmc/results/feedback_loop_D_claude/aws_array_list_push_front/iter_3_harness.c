#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 2
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 2
#endif

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t length;
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    __CPROVER_assume(current_size == 0 || current_size >= length * item_size);
    /* Avoid overflow when length increases by 1 */
    __CPROVER_assume(length < SIZE_MAX / item_size);
    __CPROVER_assume(length < SIZE_MAX);

    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;

    /* Use a real allocator so realloc can work */
    list.alloc = can_fail_allocator();

    if (current_size > 0) {
        list.data = bounded_malloc(current_size);
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
    void *val = bounded_malloc(item_size);
    __CPROVER_assume(val != NULL);

    int result = aws_array_list_push_front(&list, val);

    /* Validity invariant always holds */
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
    } else {
        /* On failure, length and current_size are unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
    }
}
