#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness(void) {
    /* 1. Declare and bound two array lists */
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    __CPROVER_assume(aws_array_list_is_bounded(&list_a,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* Use the same allocator for both lists */
    list_a.alloc = aws_default_allocator();
    list_b.alloc = list_a.alloc;

    /* Ensure item_size is the same for both lists and within bounds */
    list_a.item_size = nondet_size_t();
    __CPROVER_assume(list_a.item_size > 0);
    __CPROVER_assume(list_a.item_size <= MAX_ITEM_SIZE);
    list_b.item_size = list_a.item_size;

    /* Allocate the internal data buffers for both lists */
    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    /* Assume both lists are initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    /* 2. Save old state and a byte from each data buffer for immutability checks */
    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    struct store_byte_from_buffer a_storage;
    struct store_byte_from_buffer b_storage;

    if (list_a.data != NULL && list_a.current_size > 0) {
        save_byte_from_array((uint8_t *)list_a.data,
                             list_a.current_size,
                             &a_storage);
    }
    if (list_b.data != NULL && list_b.current_size > 0) {
        save_byte_from_array((uint8_t *)list_b.data,
                             list_b.current_size,
                             &b_storage);
    }

    /* 3. Call function under test */
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

    /* 5. Assert that the contents of the data buffers remain unchanged */
    if (list_a.data != NULL && list_a.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list_a.data, &b_storage);
    }
    if (list_b.data != NULL && list_b.current_size > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list_b.data, &a_storage);
    }

    /* 6. Assert validity invariants still hold */
    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
