#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare and bound the list structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Non‑deterministic, bounded parameters */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 3. Ensure multiplication does not overflow (precondition of the function) */
    size_t current_size;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);

    /* 4. Allocate the raw array that will back the list */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 5. Save old state (not strictly needed for a void function, but kept for pattern) */
    struct aws_array_list old = list;

    /* 6. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 7. Post‑condition checks */
    /*    All fields are defined by the specification */
    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(list.current_size == current_size);

    /* 8. No other fields exist, so nothing else to assert as unchanged */

    /* 9. The list must remain valid */
    assert(aws_array_list_is_valid(&list));
}
