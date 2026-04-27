#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_array_list_erase_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t max_item_size = nondet_size_t();
    __CPROVER_assume(max_initial_item_allocation > 0 && max_initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(max_item_size > 0 && max_item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(aws_array_list_init(&list, allocator, max_initial_item_allocation, max_item_size) == AWS_OP_SUCCESS);
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Non-deterministic index */
    size_t index = nondet_size_t();
    __CPROVER_assume(index < list.length);

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        if (index == 0) {
            assert(list.data != old.data || list.length == 0); // If length is 0, data can be anything
        } else if (index == old.length - 1) {
            assert(list.data != old.data || list.length == 0); // If length is 0, data can be anything
        } else {
            uint8_t *item_ptr = (uint8_t *)list.data + (index * list.item_size);
            uint8_t *next_item_ptr = item_ptr + list.item_size;
            size_t trailing_items = (old.length - index) - 1;
            size_t trailing_bytes = trailing_items * list.item_size;
            assert(AWS_MEM_IS_READABLE(item_ptr, trailing_bytes));
            assert(AWS_MEM_IS_READABLE(next_item_ptr, trailing_bytes));
            assert(memcmp(item_ptr, next_item_ptr, trailing_bytes) == 0);
        }
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 7. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    aws_array_list_clean_up(&list);
}
