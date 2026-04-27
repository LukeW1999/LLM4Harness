#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_array_list_pop_front_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state before calling */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    /* 3. Call function under test */
    int result = aws_array_list_pop_front(&list);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: list was non-empty, length decreases by 1 */
        assert(old_length > 0);
        assert(list.length == old_length - 1);
    } else {
        /* Failure: list was empty, nothing changes */
        assert(old_length == 0);
        assert(list.length == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old_alloc);
    assert(list.current_size == old_current_size);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
