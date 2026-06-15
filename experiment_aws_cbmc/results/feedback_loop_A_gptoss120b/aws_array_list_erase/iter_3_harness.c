#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;

    /* Assume the list is bounded and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Save a copy of the original list for later comparison */
    struct aws_array_list old = list;

    /* Choose an arbitrary index */
    size_t index = nondet_size_t();

    /* Call the function under verification */
    int result = aws_array_list_erase(&list, index);

    /* Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Successful erase: index must have been within bounds */
        assert(old.length > 0);
        assert(index < old.length);
        assert(list.length == old.length - 1);
    } else {
        /* Failure: either the list was empty or index was out of bounds */
        assert(old.length == 0 || index >= old.length);
        assert(list.length == old.length);
    }

    /* The allocator, item size, data pointer, and current size must be unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    /* The resulting list must still be valid */
    assert(aws_array_list_is_valid(&list));
}
