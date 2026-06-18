#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs and bound them */
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 2. Ensure multiplication does not overflow */
    size_t total_size;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &total_size);
    __CPROVER_assume(no_overflow);

    /* 3. Allocate the raw array */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* 4. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 5. Assert post‑conditions */
    assert(list.alloc == NULL);                     /* allocator is cleared */
    assert(list.item_size == item_size);            /* item size set */
    assert(list.length == 0);                       /* length reset */
    assert(list.data == raw_array);                 /* data points to provided array */
    assert(list.current_size == total_size);        /* capacity set correctly */

    /* 6. The list must be valid after initialization */
    assert(aws_array_list_is_valid(&list));
}
