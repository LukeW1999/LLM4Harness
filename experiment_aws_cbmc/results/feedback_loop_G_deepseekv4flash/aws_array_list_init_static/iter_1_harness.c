#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness() {
    /* Non-deterministic inputs */
    struct aws_array_list list;
    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    /* Preconditions from implementation and Doxygen */
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_size > 0);
    size_t current_size;
    int no_overflow = !aws_mul_size_checked(item_count, item_size, &current_size);
    __CPROVER_assume(no_overflow);

    /* Allocate raw_array as a valid writable region of size current_size */
    uint8_t *raw_array = malloc(current_size);
    __CPROVER_assume(raw_array != NULL);

    /* Save old state (list is uninitialized, so just save raw pointer fields for completeness) */
    struct aws_array_list old = list;
    /* We cannot assume old is valid, but we can compare against it */

    /* Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* Postconditions */
    assert(list.alloc == NULL);
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.data == raw_array);
    assert(list.current_size == current_size);
    assert(list.current_size == item_count * item_size); /* Equivalent, since multiply succeeded */

    /* Unchanged fields — all fields are explicitly set, so none are unchanged from old. */
    /* But we verify that alloc is NULL as documented. */

    /* Validity invariant */
    assert(aws_array_list_is_valid(&list));

    /* Cleanup */
    free(raw_array);
}
