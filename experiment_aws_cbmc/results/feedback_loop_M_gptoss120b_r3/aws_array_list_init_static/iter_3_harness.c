#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    void *raw_array;
    size_t item_count = nondet_size_t();
    size_t item_size   = nondet_size_t();

    /* 2. Bound scalar inputs */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 3. Compute total size and bound it to avoid overflow */
    size_t total_size = 0;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &total_size));

    /* 4. Allocate the raw array (must be non‑NULL per precondition) */
    raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* 5. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Post‑condition asserts */
    assert(list.alloc == NULL);                     /* allocator is set to NULL */
    assert(list.item_size == item_size);            /* item_size stored */
    assert(list.length == 0);                       /* length reset to 0 */
    assert(list.data == raw_array);                 /* data points to provided array */
    assert(list.current_size == total_size);        /* current_size = item_count * item_size */

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
