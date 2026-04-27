#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_swap_contents_harness(void) {
    /* Use stack-allocated structs */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* Set up lists with bounded nondeterministic values */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* Preconditions required by the function */
    /* list_a->alloc must be non-null */
    __CPROVER_assume(list_a.alloc != NULL);
    /* list_a->alloc == list_b->alloc */
    list_b.alloc = list_a.alloc;
    /* list_a->item_size == list_b->item_size */
    list_b.item_size = list_a.item_size;

    /* Ensure both lists are valid before the call */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* list_a should now contain old list_b's contents */
    assert(list_a.alloc == old_b.alloc);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.data == old_b.data);

    /* list_b should now contain old list_a's contents */
    assert(list_b.alloc == old_a.alloc);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.data == old_a.data);

    /* Validity invariants must hold after the call */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
