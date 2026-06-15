#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_init_static_harness() {
    /* 1. Declare the list and nondeterministic parameters */
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    /* 2. Apply bounds to keep the state space finite */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size  <= MAX_ITEM_SIZE);

    /* 3. Compute the total size and ensure no overflow */
    size_t current_size;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);

    /* 4. Allocate the raw backing array */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 5. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Post‑condition checks */
    /*   Fields that must be set on success */
    assert(list.alloc == NULL);
    assert(list.data == raw_array);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.current_size == current_size);

    /* 7. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
