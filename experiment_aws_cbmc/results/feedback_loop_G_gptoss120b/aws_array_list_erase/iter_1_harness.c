#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_erase_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Nondeterministic index */
    size_t index = nondet_size_t();

    /* 4. Call function under test */
    int result = aws_array_list_erase(&list, index);

    /* 5. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* length decreases by one */
        assert(list.length == old.length - 1);
        /* other fields remain unchanged */
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    } else {
        /* on failure the list is unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
    }

    /* 6. Invariant must always hold */
    assert(aws_array_list_is_valid(&list));
}
