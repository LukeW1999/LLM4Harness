#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    size_t n = nondet_size_t();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    struct aws_array_list old = list;

    aws_array_list_pop_front_n(&list, n);

    assert(aws_array_list_is_valid(&list));

    assert(list.alloc == old.alloc);
    assert(list.alloc == allocator);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        assert(list.length == old.length - n);
    }
}
