#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Invoke the function under test */
    const void *front = aws_array_list_front(&list);

    /* The function should not modify the list */
    assert(aws_array_list_is_valid(&list));

    /* If the list is non‑empty, the returned pointer must point to the first element */
    if (list.length > 0) {
        assert(front == (const void *)((const uint8_t *)list.data));
    } else {
        assert(front == NULL);
    }
}
