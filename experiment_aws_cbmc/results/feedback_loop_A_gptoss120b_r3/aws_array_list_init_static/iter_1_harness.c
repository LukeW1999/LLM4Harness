#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare the list structure (uninitialized) */
    struct aws_array_list list;

    /* 2. Non‑deterministic inputs, bounded */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size  <= MAX_ITEM_SIZE);

    /* 3. Compute total allocation size and ensure no overflow */
    size_t total_size;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &total_size));

    /* 4. Allocate the raw array that will back the list */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* 5. Save old state of the list (all fields are nondet) */
    struct aws_array_list old = list;

    /* 6. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 7. Post‑conditions on success (the function has no return value) */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.item_size == item_size);            /* item size stored */
    assert(list.length == 0);                       /* length reset to zero */
    assert(list.data == raw_array);                 /* data pointer stored */
    assert(list.current_size == total_size);        /* capacity computed correctly */

    /* 8. Unchanged fields – all fields are accounted for above */

    /* 9. Validity invariant must hold after initialization */
    assert(aws_array_list_is_valid(&list));
}
