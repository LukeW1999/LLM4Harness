#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_get_at_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *val = (uint8_t *)allocator->mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);

    size_t index = (size_t)nondet_uint64_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    int result = aws_array_list_get_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        /* Successful read: list must be unchanged and index must be within bounds */
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(index < old.length);
        assert_bytes_match(
            (uint8_t *)list.data + (index * list.item_size),
            val,
            list.item_size);
    } else {
        /* Failure: list must remain unchanged */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
