#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_array_list_init_dynamic_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_array_list list;
    struct aws_allocator alloc;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* 3. Allocate pointer members */
    ensure_array_list_has_allocated_data_member(&list);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 5. Save old state (for checking immutability) */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);

    /* 6. Assume function-specific preconditions */
    __CPROVER_assume(list.alloc == &alloc);
    __CPROVER_assume(list.data == NULL);
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(initial_item_allocation > 0);
    __CPROVER_assume(item_size > 0);

    /* 7. Call function under test */
    int result = aws_array_list_init_dynamic(&list, &alloc, initial_item_allocation, item_size);

    /* 8. Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == &alloc);
        assert(list.item_size == item_size);
        assert(list.length == 0);
        assert(list.current_size == initial_item_allocation * item_size);
        assert(list.data != NULL);
        assert(AWS_MEM_IS_WRITABLE(list.data, list.current_size));
    } else {
        assert(AWS_IS_ZEROED(list));
    }
    assert(aws_array_list_is_valid(&list));  // invariant always holds
    assert_array_list_equivalence(&list, &old, &old_byte);  // if read-only
}
