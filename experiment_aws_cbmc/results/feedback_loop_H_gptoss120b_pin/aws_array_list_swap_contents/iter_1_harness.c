#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 2. Set up preconditions required by the function */
    list_a.alloc = aws_default_allocator();
    list_b.alloc = list_a.alloc;                     /* same allocator */
    __CPROVER_assume(list_a.alloc != NULL);         /* non‑null allocator */

    __CPROVER_assume(list_a.item_size == list_b.item_size); /* same item size */
    __CPROVER_assume(&list_a != &list_b);           /* distinct objects */

    /* 3. Save old state BEFORE calling */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 4. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 5. Assert postconditions (swap semantics) */
    /* swapped allocator pointers (they were equal, remain equal) */
    assert(list_a.alloc == old_b.alloc);
    assert(list_b.alloc == old_a.alloc);
    assert(list_a.alloc == list_b.alloc);

    /* swapped item sizes */
    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);
    assert(list_a.item_size == list_b.item_size);

    /* swapped current_size */
    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    /* swapped length */
    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);

    /* swapped data pointers */
    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* 6. Assert validity invariants always hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
