#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(list.data, list.current_size, &old_bytes);

    size_t index = nondet_size_t();

    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        /* Success implies the index was within bounds */
        __CPROVER_assert(index < old.length, "index within bounds for success");

        /* Length decreased by one */
        assert(list.length == old.length - 1);
        /* Capacity unchanged */
        assert(list.current_size == old.current_size);
        /* Allocator, item size, and data pointer unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure: list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        /* Data buffer unchanged */
        assert_array_list_equivalence(&list, &old, &old_bytes);
        /* Validity invariant */
        assert(aws_array_list_is_valid(&list));
    }
}
