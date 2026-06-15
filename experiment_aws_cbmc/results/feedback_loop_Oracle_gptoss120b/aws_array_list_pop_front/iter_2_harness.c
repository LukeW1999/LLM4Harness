#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Initialize a list with bounded capacity and item size. */
    aws_array_list_init(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    /* Nondeterministically set the current length (must be ≤ capacity). */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= list.capacity);
    list.length = len;

    /* Capture pre‑state. */
    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* Call the function under verification. */
    int ret = aws_array_list_pop_front(&list);

    /* Post‑condition: return value / error code correctness. */
    if (old_length > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(aws_array_list_length(&list) == old_length);
    }

    /* Capacity must remain unchanged. */
    assert(aws_array_list_capacity(&list) == old_capacity);

    /* Frame conditions: allocator, data pointer, and overall validity unchanged. */
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);
    assert(aws_array_list_is_valid(&list));

    /* Clean up. */
    aws_array_list_clean_up(&list);
}
