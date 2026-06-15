#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_length_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for frame condition checks */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes = {0};
    if (list.data) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &old_bytes);
    }

    /* 3. Call function under test */
    size_t len = aws_array_list_length(&list);

    /* 4. Post‑condition checks */
    /* length invariant: returned length must equal stored length */
    assert(len == list.length);

    /* frame condition: fields that must not change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);
    if (list.data) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_bytes);
    }

    /* validity predicate must hold after the call */
    assert(aws_array_list_is_valid(&list));
}
