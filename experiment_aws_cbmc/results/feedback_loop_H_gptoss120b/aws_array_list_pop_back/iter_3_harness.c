#include <assert.h>
#include <stddef.h>
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_pop_back_harness() {
    struct aws_array_list list;

    /* list is bounded and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* make length nondeterministic but within bounds */
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    /* out may be NULL or point to a fresh buffer of item_size bytes */
    void *out = nondet_void_ptr();
    __CPROVER_assume(out == NULL ||
                     __CPROVER_is_fresh(out, list.item_size));

    struct aws_array_list old = list;

    int result = aws_array_list_pop_back(&list, out);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    assert(aws_array_list_is_valid(&list));
}
