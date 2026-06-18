#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare the list structure */
    struct aws_array_list list;

    /* 2. Non‑deterministic inputs, bounded */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size  > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size  <= MAX_ITEM_SIZE);

    /* 3. Ensure multiplication does not overflow */
    size_t current_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &current_size));

    /* 4. Allocate the raw array (pre‑allocated storage) */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);   /* matches the fatal precondition */

    /* 5. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Post‑conditions on success (function has no error return) */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.item_size == item_size);            /* item size stored */
    assert(list.length == 0);                       /* list starts empty */
    assert(list.data == raw_array);                 /* data points to provided array */
    assert(list.current_size == current_size);      /* total byte size matches allocation */

    /* 7. Invariant: the list must be valid after initialization */
    assert(aws_array_list_is_valid(&list));
}
