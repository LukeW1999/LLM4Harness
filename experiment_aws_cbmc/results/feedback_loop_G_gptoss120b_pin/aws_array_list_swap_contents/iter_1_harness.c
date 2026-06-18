#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare and bound the two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    /* bound the structures (use the same bounds for both) */
    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* allocate the internal data buffers */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* set allocator to a non‑null common allocator and make them equal */
    struct aws_allocator *alloc = aws_default_allocator();
    list_a.alloc = alloc;
    list_b.alloc = alloc;

    /* make item_size equal and bounded */
    size_t item_sz = nondet_size_t();
    __CPROVER_assume(item_sz > 0 && item_sz <= MAX_ITEM_SIZE);
    list_a.item_size = item_sz;
    list_b.item_size = item_sz;

    /* ensure the lists are valid according to the library invariant */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 2. Save old state before the call */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    /* save a byte from each data buffer for later equivalence checks */
    struct store_byte_from_buffer storage_a;
    struct store_byte_from_buffer storage_b;
    if (old_a.data != NULL && old_a.current_size > 0) {
        save_byte_from_array((uint8_t *)old_a.data, old_a.current_size, &storage_a);
    }
    if (old_b.data != NULL && old_b.current_size > 0) {
        save_byte_from_array((uint8_t *)old_b.data, old_b.current_size, &storage_b);
    }

    /* 3. Call the function under test */
    aws_array_list_swap_contents(&list_a, &list_b);

    /* 4. Assert postconditions: fields are swapped */
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

    /* 5. Assert that allocator and item_size remain equal between the two lists */
    assert(list_a.alloc == list_b.alloc);
    assert(list_a.item_size == list_b.item_size);

    /* 6. Assert that the contents of the buffers are preserved after the swap */
    assert_array_list_equivalence(&list_a, &old_b, &storage_b);
    assert_array_list_equivalence(&list_b, &old_a, &storage_a);

    /* 7. Assert validity invariants still hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
