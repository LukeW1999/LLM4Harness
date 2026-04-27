#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_array_list_init_static_harness() {
    // Stack-allocated list struct
    struct aws_array_list list;

    // Nondeterministic item_count and item_size, both > 0 (preconditions)
    size_t item_count;
    size_t item_size;
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);

    // Bound sizes to avoid state space explosion
    __CPROVER_assume(item_count <= 2);
    __CPROVER_assume(item_size <= 2);

    // Ensure no overflow: item_count * item_size must not overflow
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    // Allocate raw_array with sufficient size
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    // Call the function under test
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    // === Post-condition assertions ===
    __CPROVER_assert(list.alloc == NULL, "alloc is NULL");
    __CPROVER_assert(list.current_size == current_size, "current_size correct");
    __CPROVER_assert(list.item_size == item_size, "item_size correct");
    __CPROVER_assert(list.length == 0, "length is 0");
    __CPROVER_assert(list.data == raw_array, "data points to raw_array");

    // === Validity invariant ===
    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid");
}
