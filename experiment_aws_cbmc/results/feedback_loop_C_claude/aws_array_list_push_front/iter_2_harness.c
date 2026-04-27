#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    /* Ensure item_size is non-zero and bounded */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);

    /* Bound the list to keep verification tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* Ensure allocated data member */
    ensure_array_list_has_allocated_data_member(&list);

    /* Assume the list is valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate val with item_size bytes of readable memory */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;

    /* Save a copy of val's content for later comparison */
    void *val_copy = malloc(list.item_size);
    __CPROVER_assume(val_copy != NULL);
    memcpy(val_copy, val, list.item_size);

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
        /* Length must be unchanged */
        assert(list.length == old_length);

        /* current_size must be unchanged */
        assert(list.current_size == old_current_size);
    }
}
