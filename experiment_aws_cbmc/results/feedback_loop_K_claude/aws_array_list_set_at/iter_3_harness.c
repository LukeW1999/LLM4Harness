#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEM_SIZE 4
#define MAX_INITIAL_ITEM_ALLOCATION 4

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 &&
                     initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Manually initialize the list to avoid dynamic allocation issues */
    list.alloc = allocator;
    list.item_size = item_size;
    list.length = 0;
    list.current_size = initial_item_allocation * item_size;
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.data != NULL);

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Set up val */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);

    /* Set up index - allow both in-bounds and out-of-bounds */
    size_t index;
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION + 1);

    /* Capture pre-call state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call the function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert(list.data != NULL);

        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }

        assert(list.length * list.item_size <= list.current_size);

        void *stored = (void *)((uint8_t *)list.data + (list.item_size * index));
        assert(memcmp(stored, val, list.item_size) == 0);
    } else {
        assert(list.length == old_length);
    }

    aws_array_list_clean_up(&list);
}
