#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_swap_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the original list for later comparison */
    struct aws_array_list old = list;

    /* 2. Choose nondeterministic indices within bounds */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* 3. Save the original elements at positions a and b */
    uint8_t old_a[MAX_ITEM_SIZE];
    uint8_t old_b[MAX_ITEM_SIZE];
    /* item_size is guaranteed to be <= MAX_ITEM_SIZE by the bounding predicate */
    (void)aws_array_list_get_at(&list, old_a, a);
    (void)aws_array_list_get_at(&list, old_b, b);

    /* 4. Call the function under test */
    aws_array_list_swap(&list, a, b);

    /* 5. Verify that the elements have been swapped */
    uint8_t new_a[MAX_ITEM_SIZE];
    uint8_t new_b[MAX_ITEM_SIZE];
    (void)aws_array_list_get_at(&list, new_a, a);
    (void)aws_array_list_get_at(&list, new_b, b);

    /* a's new value should equal old b, and b's new value should equal old a */
    assert_bytes_match(new_a, old_b, list.item_size);
    assert_bytes_match(new_b, old_a, list.item_size);

    /* 6. Unchanged fields */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);

    /* 7. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
