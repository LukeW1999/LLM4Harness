#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_array_list_pop_front_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic item size and capacity within small bounds */
    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= 8);
    size_t capacity;
    __CPROVER_assume(capacity > 0 && capacity <= 10);

    struct aws_array_list list;
    int init_result = aws_array_list_init(&list, alloc, capacity, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(list.data != NULL);               /* allocation must have succeeded */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic length bounded by capacity */
    size_t length;
    __CPROVER_assume(length <= capacity);
    list.length = length;

    /* preserve old state */
    void *old_data = list.data;
    size_t old_item_size = list.item_size;
    size_t old_capacity = aws_array_list_capacity(&list);
    size_t old_length = aws_array_list_length(&list);

    /* call under verification */
    int result = aws_array_list_pop_front(&list);

    /* result must be either success (0) or error (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* if the list was empty, pop_front must fail; otherwise it must succeed */
    if (old_length == 0) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
    }

    /* the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* allocator, item size, data pointer, and capacity are unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(aws_array_list_capacity(&list) == old_capacity);

    /* length updates correctly */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }
}
