#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_ptr_harness() {
    /* data structure */
    struct aws_array_list list; /* Precondition: list is non-null */

    /* parameters */
    size_t index;
    void *val;

    /* assumptions */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION); /* Limiting index to a reasonable value */

    /* save old state */
    struct aws_array_list old = list;

    /* perform operation under verification */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(val == (void *)((uint8_t *)list.data + (list.item_size * index)));
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    } else {
        assert(val == NULL);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
    }

    /* validity invariants */
    assert(aws_array_list_is_valid(&list));
}
