#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 4
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 4
#endif

void aws_array_list_set_at_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;

    /* Use bounded setup */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    list.item_size = item_size;
    list.alloc = aws_default_allocator();

    /* Set current_size and length with bounds */
    size_t current_size;
    __CPROVER_assume(current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    /* current_size must be a multiple of item_size or zero */
    __CPROVER_assume(current_size == 0 || (current_size % item_size == 0));
    list.current_size = current_size;

    size_t length;
    /* length in items, must fit within current_size */
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(length * item_size <= current_size);
    list.length = length;

    /* Allocate data if current_size > 0 */
    if (current_size > 0) {
        list.data = malloc(current_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Bound the index */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    /* 3. Create a valid val pointer with item_size bytes readable */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* 4. Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 5. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 6. Assert postconditions */

    /* Unchanged fields regardless of result */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* On success: data was written at index, length may have increased */
        assert(list.data != NULL);

        /* If index was >= old length, length should be index + 1 */
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            /* If index was within bounds, length stays the same */
            assert(list.length == old_length);
        }

        /* current_size must be at least enough to hold index+1 elements */
        assert(list.current_size >= (index + 1) * list.item_size);

        /* The value at index should match val */
        assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + (list.item_size * index), list.item_size));
        assert_bytes_match((uint8_t *)list.data + (list.item_size * index), val, list.item_size);

        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* On failure: list should still be valid */
        assert(aws_array_list_is_valid(&list));
    }
}
