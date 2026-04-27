#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    void *raw_array = (void *)nondet_ptr();
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(raw_array != NULL);
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_array_list old_list = list;

    /* 3. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Since the function is void, we don't have a return value to check */
    /* Success path assertions */
    assert(list.alloc == NULL);
    assert(list.current_size == current_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to check as they are all set by the function */

    /* 6. Assert validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
