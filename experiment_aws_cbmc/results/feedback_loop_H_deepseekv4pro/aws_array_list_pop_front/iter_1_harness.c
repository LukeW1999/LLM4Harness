#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_pop_front_harness() {
    struct aws_array_list list;

    /* Assume the list is bounded and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_pop_front(&list);

    /* Postconditions based on whether the list was empty */
    if (old.length > 0) {
        /* Should succeed and length decreases by one */
        assert(result == AWS_OP_SUCCESS);
        assert(list.length == old.length - 1);
    } else {
        /* Should fail and length remains unchanged */
        assert(result != AWS_OP_SUCCESS);
        assert(list.length == old.length);
    }

    /* All other fields remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* The list must remain valid after the operation */
    assert(aws_array_list_is_valid(&list));
}
