#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_pop_back_harness(void) {
    /* 1. Declare and set up the list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;

    /* 3. Optionally provide a buffer to receive the popped element */
    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = bounded_malloc(list.item_size);
    }

    /* 4. Call the function under test */
    int result = aws_array_list_pop_back(&list, (void *)val);

    /* 5. Assert postconditions */

    /* item_size must never change */
    assert(list.item_size == old_item_size);

    /* alloc must never change */
    assert(list.alloc == old_alloc);

    if (result == AWS_OP_SUCCESS) {
        /* On success: list was non-empty, length decreases by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
        /* current_size should not change on pop */
        assert(list.current_size == old_current_size);
        /* data pointer should not change on pop */
        assert(list.data == old_data);
    } else {
        /* On failure: list was empty, nothing changes */
        assert(old_length == 0);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        assert(list.data == old_data);
    }

    /* 6. Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
