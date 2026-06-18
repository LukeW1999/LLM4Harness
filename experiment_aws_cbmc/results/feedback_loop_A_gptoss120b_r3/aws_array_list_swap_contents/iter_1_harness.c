#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
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

    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 2. Impose the fatal preconditions required by the implementation */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    list_a.alloc = alloc;
    list_b.alloc = alloc;
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b); /* distinct objects */

    /* 3. Save old state for both lists */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    struct store_byte_from_buffer old_a_bytes = {0};
    struct store_byte_from_buffer old_b_bytes = {0};

    if (list_a.data != NULL && list_a.current_size > 0) {
        save_byte_from_array((uint8_t *)list_a.data,
                             list_a.current_size,
                             &old_a_bytes);
    }
    if (list_b.data != NULL && list_b.current_size > 0) {
        save_byte_from_array((uint8_t *)list_b.data,
                             list_b.current_size,
                             &old_b_bytes);
    }

    /* 4. Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 5. Post‑condition: the contents of the two lists are swapped */
    assert_array_list_equivalence(&list_a, &old_b, &old_b_bytes);
    assert_array_list_equivalence(&list_b, &old_a, &old_a_bytes);

    /* 6. The allocator pointer is unchanged (both lists used the same allocator) */
    assert(list_a.alloc == alloc);
    assert(list_b.alloc == alloc);

    /* 7. Validity invariants must still hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
