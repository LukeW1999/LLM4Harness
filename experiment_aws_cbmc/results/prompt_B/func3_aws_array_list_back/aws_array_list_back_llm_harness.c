#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/proof_allocators.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_array_list_back_harness() {
    struct aws_array_list list;
    size_t max_initial_item_allocation = nondet_size_t();
    size_t initial_item_size = nondet_size_t();

    // Initialize the array list with non-deterministic values
    ensure_memory_is_forced_to_be_in_bounded_allocation_pool(&list.data, max_initial_item_allocation * initial_item_size);
    __CPROVER_assume(aws_array_list_is_bounded(&list, max_initial_item_allocation, initial_item_size));
    make_and_validate_bounded_array_list(&list, max_initial_item_allocation, initial_item_size);

    // Non-deterministically choose the length of the list
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= max_initial_item_allocation);

    // Allocate memory for val
    void *val = can_fail_malloc(initial_item_size);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, initial_item_size));

    // Save old state before the call
    struct aws_array_list old_list = list;

    // Call the function under verification
    int result = aws_array_list_back(&list, val);

    // Assert postconditions
    assert(aws_array_list_is_valid(&list));
    assert(memcmp(&list, &old_list, sizeof(struct aws_array_list)) == 0); // Ensure list is unchanged

    if (result == AWS_OP_SUCCESS) {
        // If the list is non-empty, val should contain the last item
        assert(list.length > 0);
        size_t last_item_offset = list.item_size * (list.length - 1);
        assert(memcmp(val, (void *)((uint8_t *)list.data + last_item_offset), list.item_size) == 0);
    } else {
        // If the list is empty, the function should return AWS_OP_ERR
        assert(list.length == 0);
        assert(result == AWS_OP_ERR);
    }
}
