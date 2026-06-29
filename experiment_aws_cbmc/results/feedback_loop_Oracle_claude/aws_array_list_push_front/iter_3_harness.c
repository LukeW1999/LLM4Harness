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

    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t initial_item_allocation;
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    int init_result = aws_array_list_init_dynamic(&list, allocator, initial_item_allocation, item_size);
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }

    /* Nondeterministically set the length to something valid */
    size_t initial_length;
    __CPROVER_assume(initial_length <= initial_item_allocation);
    list.length = initial_length;

    /* Allocate val with item_size bytes */
    uint8_t *val = malloc(item_size);
    if (val == NULL) {
        aws_array_list_clean_up(&list);
        return;
    }

    /* Save pre-call state */
    size_t old_length = list.length;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    assert(aws_array_list_is_valid(&list));

    int result = aws_array_list_push_front(&list, val);

    /* Check postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
        assert(aws_array_list_is_valid(&list));
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(aws_array_list_capacity(&list) >= list.length);
        assert(list.current_size >= list.length * list.item_size);
    } else {
        assert(list.length == old_length);
        assert(list.item_size == old_item_size);
        assert(list.alloc == old_alloc);
        assert(aws_array_list_is_valid(&list));
    }

    assert(list.item_size == old_item_size);
    assert(list.alloc == old_alloc);
    assert(list.length <= aws_array_list_capacity(&list));
    assert(list.current_size >= list.length * list.item_size);

    free(val);
    aws_array_list_clean_up(&list);
}
