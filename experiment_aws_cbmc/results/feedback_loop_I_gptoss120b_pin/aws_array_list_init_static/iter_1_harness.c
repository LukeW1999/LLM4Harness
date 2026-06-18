#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness() {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Compute total size and assume no overflow */
    size_t total_size = 0;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &total_size);
    __CPROVER_assume(no_overflow);
    __CPROVER_assume(total_size > 0);

    /* Allocate raw array (must be non‑NULL per precondition) */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* 2. Declare the list structure */
    struct aws_array_list list;

    /* Save old state (not really needed for this function, but kept for pattern) */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 4. Assert postconditions */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.item_size == item_size);            /* item size stored */
    assert(list.length == 0);                       /* length reset to 0 */
    assert(list.data == raw_array);                 /* data points to provided array */
    assert(list.current_size == total_size);        /* current_size = item_count * item_size */

    /* 5. Unchanged fields – all fields are explicitly set, so no additional asserts needed */

    /* 6. Validity invariant must hold */
    assert(aws_array_list_is_valid(&list));
}
