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

    /* 2. Non‑deterministic inputs with appropriate bounds */
    size_t item_count = nondet_size_t();
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 3. Compute the total size and ensure no overflow */
    size_t total_size = 0;
    bool no_overflow = !aws_mul_size_checked(item_count, item_size, &total_size);
    __CPROVER_assume(no_overflow);
    __CPROVER_assume(total_size > 0); /* malloc must receive non‑zero size */

    /* 4. Allocate the raw array that will back the list */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(raw_array, total_size));

    /* 5. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Post‑conditions */
    /* All fields are explicitly set by the function */
    assert(list.alloc == NULL);
    assert(list.current_size == total_size);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);

    /* 7. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
