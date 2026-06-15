#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare the list and bound it */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 2. Non‑deterministic parameters, bounded */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size  <= MAX_ITEM_SIZE);

    /* 3. Ensure multiplication does not overflow */
    size_t current_size;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);

    /* 4. Allocate the raw array that will back the list */
    void *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* 5. Save old state (all fields are expected to change) */
    struct aws_array_list old = list;

    /* 6. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 7. Post‑conditions */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.length == 0);                       /* length is reset */
    assert(list.item_size == item_size);            /* item size stored */
    assert(list.current_size == current_size);     /* total byte size stored */
    assert(list.data == raw_array);                 /* data pointer stored */

    /* 8. No other fields exist, so nothing else to assert unchanged */

    /* 9. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
