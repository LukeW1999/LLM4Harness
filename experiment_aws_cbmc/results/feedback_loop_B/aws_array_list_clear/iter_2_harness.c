#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <assert.h>

void aws_array_list_clear_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    size_t initial_item_allocation = nondet_size_t();
    size_t item_size = nondet_size_t();
    __CPROVER_assume(initial_item_allocation > 0 && initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    if (aws_array_list_init(&list, initial_item_allocation, item_size)) {
        return; // Skip if initialization fails
    }

    /* 2. Populate the list with some elements */
    size_t num_elements = nondet_size_t();
    __CPROVER_assume(num_elements <= initial_item_allocation);
    for (size_t i = 0; i < num_elements; i++) {
        uint8_t *item = bounded_malloc(item_size);
        __CPROVER_assume(item != NULL);
        if (aws_array_list_push_back(&list, item)) {
            free(item);
            break; // Stop if push_back fails
        }
    }

    /* 3. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old = list;

    /* 4. Call function under test */
    aws_array_list_clear(&list);

    /* 5. Assert postconditions for clear operation */
    assert(list.length == 0); // Length should be zero after clear
    assert(list.data != NULL || list.alloc == 0); // Data pointer should be non-null if alloc is non-zero
    assert(list.alloc == old.alloc); // Allocation should not change
    assert(list.current_size == old.current_size); // Current size should not change
    assert(list.item_size == old.item_size); // Item size should not change

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));

    /* Clean up allocated memory */
    aws_array_list_clean_up(&list);
}
