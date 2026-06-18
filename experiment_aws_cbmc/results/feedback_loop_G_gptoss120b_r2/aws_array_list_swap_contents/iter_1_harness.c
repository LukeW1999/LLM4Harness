#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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

    /* 2. Assume the fatal preconditions required by the function */
    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    /* 3. Save old state */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;

    if (list_a.data != NULL && list_a.current_size > 0) {
        save_byte_from_array(list_a.data, list_a.current_size, &storage_a);
    }
    if (list_b.data != NULL && list_b.current_size > 0) {
        save_byte_from_array(list_b.data, list_b.current_size, &storage_b);
    }

    /* 4. Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 5. Postconditions: fields are swapped */
    assert(list_a.alloc == old_b.alloc);
    assert(list_b.alloc == old_a.alloc);

    assert(list_a.item_size == old_b.item_size);
    assert(list_b.item_size == old_a.item_size);

    assert(list_a.current_size == old_b.current_size);
    assert(list_b.current_size == old_a.current_size);

    assert(list_a.length == old_b.length);
    assert(list_b.length == old_a.length);

    assert(list_a.data == old_b.data);
    assert(list_b.data == old_a.data);

    /* 6. Data buffers themselves are unchanged */
    if (old_b.data != NULL && old_b.current_size > 0) {
        assert_array_list_equivalence(&list_a, &old_b, &storage_b);
    }
    if (old_a.data != NULL && old_a.current_size > 0) {
        assert_array_list_equivalence(&list_b, &old_a, &storage_a);
    }

    /* 7. Validity invariants must still hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
