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

void aws_array_list_push_front_harness(void) {
    struct aws_array_list list;

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the list */
    if (aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    /* Ensure the list is valid after init */
    assert(aws_array_list_is_valid(&list));

    /* Create a val buffer of item_size bytes */
    uint8_t *val = malloc(item_size);
    if (val == NULL) {
        aws_array_list_clean_up(&list);
        return;
    }

    /* Save old state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* Call function under test */
    int result = aws_array_list_push_front(&list, val);

    /* Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(list.current_size >= list.length * list.item_size);
        assert(list.data != NULL);
    } else {
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
