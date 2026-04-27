#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministic input */
    size_t index = nondet_size_t();

    /* Ensure index is within bounds for testing */
    __CPROVER_assume(index < old.length || old.length == 0);

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: element at index is removed */
        assert(list.length == old.length - 1);
        assert(AWS_MEM_IS_READABLE(list.data, list.length * list.item_size));
        if (index == 0) {
            /* Removing front element */
            uint8_t *new_start = (uint8_t *)list.data;
            uint8_t *old_start = (uint8_t *)old.data + list.item_size;
            size_t bytes_to_check = (old.length - 1) * list.item_size;
            assert_bytes_match(new_start, old_start, bytes_to_check);
        } else if (index == (old.length - 1)) {
            /* Removing back element */
            uint8_t *last_old_element = (uint8_t *)old.data + (old.length - 1) * list.item_size;
            assert(!AWS_MEM_IS_READABLE(last_old_element, list.item_size));
        } else {
            /* Removing middle element */
            uint8_t *item_ptr = (uint8_t *)list.data + (index * list.item_size);
            uint8_t *next_item_ptr = item_ptr + list.item_size;
            size_t trailing_items = (old.length - index) - 1;
            size_t trailing_bytes = trailing_items * list.item_size;
            assert_bytes_match(item_ptr, next_item_ptr, trailing_bytes);
        }
    } else {
        /* Failure: struct unchanged on failure */
        assert(list.alloc == old.alloc);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
