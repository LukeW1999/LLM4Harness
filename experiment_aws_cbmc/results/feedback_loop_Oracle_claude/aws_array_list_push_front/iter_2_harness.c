#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 4
#define MAX_ITEM_SIZE 8

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;

    /* Use a concrete allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministically choose item_size and initial capacity */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    /* Initialize the list properly */
    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Nondeterministically set the length to something valid */
    size_t initial_length;
    __CPROVER_assume(initial_length <= initial_item_allocation);
    list.length = initial_length;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate val with item_size bytes */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    int result = aws_array_list_push_front(&list, val);

    /* 1. Return value / error code correctness */
    if (result == AWS_OP_SUCCESS) {
        /* On success, length must have increased by 1 */
        assert(list.length == old_length + 1);
        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
        /* item_size must not change */
        assert(list.item_size == old_item_size);
        /* alloc must not change */
        assert(list.alloc == old_alloc);
        /* capacity must be at least the new length */
        assert(aws_array_list_capacity(&list) >= list.length);
        /* current_size must be at least item_size * length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure, length must not have changed */
        assert(list.length == old_length);
        /* item_size must not change */
        assert(list.item_size == old_item_size);
        /* alloc must not change */
        assert(list.alloc == old_alloc);
        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));
    }

    /* 2. Output buffer length/capacity invariants */
    if (list.item_size > 0) {
        assert(list.current_size >= list.length * list.item_size);
    }
    /* length must not exceed capacity */
    assert(list.length <= aws_array_list_capacity(&list));

    /* 3. Frame conditions */
    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);

    free(val);
    aws_array_list_clean_up(&list);
}
