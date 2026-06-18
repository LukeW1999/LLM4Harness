#include <assert.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t item_size = nondet_size_t();
    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(initial_capacity <= 10);

    aws_array_list_init(&list, allocator, item_size, initial_capacity);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    int result = aws_array_list_clear(&list);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == 0);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    }

    assert(aws_array_list_is_valid(&list));
}
