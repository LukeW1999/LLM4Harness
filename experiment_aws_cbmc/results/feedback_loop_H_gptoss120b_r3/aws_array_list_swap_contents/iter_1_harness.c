#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare and bound the two array lists */
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

    /* 2. Set allocator and satisfy preconditions */
    list_a.alloc = aws_default_allocator();
    list_b.alloc = aws_default_allocator();

    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 3. Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* 4. Call function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 5. Post‑condition: fields that must stay the same */
    assert(list_a.alloc == old_a.alloc);
    assert(list_b.alloc == old_b.alloc);
    assert(list_a.item_size == old_a.item_size);
    assert(list_b.item_size == old_b.item_size);

    /* 6. Post‑condition: fields that must be swapped */
    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);

    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* 7. Validity invariants must hold after the swap */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
