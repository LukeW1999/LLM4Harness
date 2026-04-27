#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_array_list_pop_front_n_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_init(&list, allocator, max_initial_item_allocation, max_item_size) == AWS_OP_SUCCESS);
    ensure_array_list_has_allocated_data_member(&list, allocator);
    size_t initial_length = nondet_size_t();
    __CPROVER_assume(initial_length <= max_initial_item_allocation);
    list.length = initial_length;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data;
    save_byte_from_array(list.data, list.current_size, &old_data);

    /* 3. Non-deterministic input */
    size_t n = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_pop_front_n(&list, n);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        if (n >= old.length) {
            /* Case where list is cleared */
            assert(list.length == 0);
            assert(list.current_size == old.current_size); /* Capacity remains unchanged */
        } else if (n > 0) {
            /* Case where some elements are popped */
            size_t popping_bytes = old.item_size * n;
            size_t remaining_items = old.length - n;
            size_t remaining_bytes = remaining_items * old.item_size;
            assert(list.length == remaining_items);
            assert(AWS_MEM_IS_READABLE(list.data, remaining_bytes));
            assert(AWS_MEM_IS_WRITABLE(list.data, remaining_bytes));
            assert_bytes_match(list.data, (uint8_t *)old.data + popping_bytes, remaining_bytes);
#ifdef DEBUG_BUILD
            assert(AWS_MEM_IS_READABLE((uint8_t *)list.data + remaining_bytes, popping_bytes));
            uint8_t debug_fill = AWS_ARRAY_LIST_DEBUG_FILL;
            assert_bytes_match((uint8_t *)list.data + remaining_bytes, &debug_fill, popping_bytes);
#endif
        } else {
            /* Case where no elements are popped */
            assert(list.length == old.length);
            assert(AWS_MEM_IS_READABLE(list.data, old.current_size));
            assert(AWS_MEM_IS_WRITABLE(list.data, old.current_size));
            assert_byte_from_buffer_matches(list.data, &old_data);
        }
    } else {
        /* Case where operation failed */
        assert(list.length == old.length);
        assert(AWS_MEM_IS_READABLE(list.data, old.current_size));
        assert(AWS_MEM_IS_WRITABLE(list.data, old.current_size));
        assert_byte_from_buffer_matches(list.data, &old_data);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size); /* Capacity remains unchanged */
    assert(list.data == old.data); /* Pointer to data remains unchanged */

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
