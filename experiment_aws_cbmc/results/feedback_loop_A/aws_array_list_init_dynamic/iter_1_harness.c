#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    struct aws_allocator *alloc = can_fail_allocator_new();
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Bound assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    int result = aws_array_list_init_dynamic(&list, alloc, initial_item_allocation, item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);
        if (initial_item_allocation > 0) {
            assert(list.data != NULL);
#ifdef DEBUG_BUILD
            assert(AWS_MEM_IS_READABLE(list.data, list.current_size));
#endif
        } else {
            assert(list.data == NULL);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(list.alloc == alloc);
    assert(list.item_size == item_size);

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Cleanup */
    if (result == AWS_OP_SUCCESS) {
        aws_array_list_clean_up(&list);
    }
    aws_allocator_destroy(alloc);
}
