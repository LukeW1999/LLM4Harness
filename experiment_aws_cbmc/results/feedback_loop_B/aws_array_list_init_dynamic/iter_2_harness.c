#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Bound the inputs */
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        size_t expected_allocation_size = 0;
        __CPROVER_assume(!aws_mul_size_checked(initial_item_allocation, item_size, &expected_allocation_size));
        assert(list.current_size == expected_allocation_size);
        assert(list.item_size == item_size);
        assert(list.alloc == alloc);
        if (expected_allocation_size > 0) {
            assert(list.data != NULL);
#ifdef DEBUG_BUILD
            uint8_t *data = (uint8_t *)list.data;
            for (size_t i = 0; i < expected_allocation_size; i++) {
                assert(data[i] == AWS_ARRAY_LIST_DEBUG_FILL);
            }
#endif
        } else {
            assert(list.data == NULL);
        }
    } else {
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.length == 0);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
