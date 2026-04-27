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

    /* Use the helper to properly initialize the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* item_size must be positive */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* Allocate val with item_size bytes */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* Call the function under test */
    int result = aws_array_list_push_front(&list, val);

    /* alloc and item_size never change */
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Length must have increased by 1 */
        assert(list.length == old_length + 1);

        /* The list must still be valid */
        assert(aws_array_list_is_valid(&list));

        /* data must be non-NULL */
        assert(list.data != NULL);

        /* current_size must be at least enough to hold the new length */
        assert(list.current_size >= list.length * list.item_size);
    } else {
        /* On failure, length must be unchanged */
        assert(list.length == old_length);

        /* current_size must be unchanged */
        assert(list.current_size == old_current_size);
    }
}
