#include <assert.h>
#include <stddef.h>
#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;

    /* list is bounded and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* keep a copy of the original state */
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(list.data, list.current_size, &old_bytes);

    /* nondet index with a nondet decision to make both outcomes reachable */
    size_t index = nondet_size_t();
    bool take_success_path = nondet_bool();

    if (take_success_path) {
        /* force a successful erase: list must have at least one element and
         * index must be within bounds */
        __CPROVER_assume(old.length > 0);
        __CPROVER_assume(index < old.length);
    } else {
        /* force a failure: either the list is empty or index is out of bounds */
        __CPROVER_assume(old.length == 0 || index >= old.length);
    }

    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        /* Success case checks */
        assert(index < old.length);
        assert(list.length == old.length - 1);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    } else {
        /* Failure case checks: list must be unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert_array_list_equivalence(&list, &old, &old_bytes);
        assert(aws_array_list_is_valid(&list));
    }
}
