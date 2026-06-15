#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare the two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* 2. Bound them */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* 3. Allocate internal buffers */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* 4. Use the default allocator for both lists */
    list_a.alloc = aws_default_allocator();
    list_b.alloc = aws_default_allocator();

    /* 5. Assume pre‑conditions required by the implementation */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);               /* distinct objects */

    /* 6. Assume the lists are initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 7. Save old state for later equivalence checks */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;

    if (list_a.data != NULL && list_a.item_size != 0 && list_a.length > 0) {
        save_byte_from_array((uint8_t *)list_a.data,
                             list_a.item_size * list_a.length,
                             &storage_a);
    }

    if (list_b.data != NULL && list_b.item_size != 0 && list_b.length > 0) {
        save_byte_from_array((uint8_t *)list_b.data,
                             list_b.item_size * list_b.length,
                             &storage_b);
    }

    /* 8. Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 9. Post‑conditions: both lists must remain valid */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));

    /* 10. After swapping, each list must be equivalent to the other's old state */
    assert_array_list_equivalence(&list_a, &old_b, &storage_b);
    assert_array_list_equivalence(&list_b, &old_a, &storage_a);
}
