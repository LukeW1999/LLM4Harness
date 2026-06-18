#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare inputs */
    struct aws_array_list list;
    void *raw_array;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* 2. Apply precondition bounds */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* 3. Compute total size and ensure no overflow */
    size_t current_size = 0;
    bool no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);
    __CPROVER_assume(current_size > 0);

    /* 4. Allocate raw array */
    raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);
    /* Ensure the allocated memory is writable for the whole region */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(raw_array, current_size));

    /* 5. Call function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 6. Postconditions */

    /* Changed fields */
    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.current_size == current_size);
    assert(list.data == raw_array);

    /* Unchanged fields – none other than those explicitly set */

    /* 7. Validity invariant */
    assert(aws_array_list_is_valid(&list));
}
