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
    struct aws_array_list list;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_storage = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_storage);
    }

    size_t len = aws_array_list_length(&list);

    /* Post‑conditions */
    assert(len == list.length);
    assert(aws_array_list_is_valid(&list));

    /* The length function must not modify the list */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (list.data) {
        assert_byte_from_buffer_matches(list.data, &old_storage);
    }
}
