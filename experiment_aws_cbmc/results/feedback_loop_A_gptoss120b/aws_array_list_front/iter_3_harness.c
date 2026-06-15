#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;

    /* Set up a list with allocated data and bounded size */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Preserve the original state for later comparison */
    struct aws_array_list old = list;

    /* Call the function under test */
    const void *front = aws_array_list_front(&list);

    /* Global invariant must still hold */
    assert(aws_array_list_is_valid(&list));

    /* The list structure must be unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.data == old.data);

    /* Front pointer semantics */
    if (list.length == 0) {
        assert(front == NULL);
    } else {
        assert(front == list.data);
    }
}
