#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    size_t n = nondet_size_t();

    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);

    ensure_array_list_has_allocated_data_member(&list);

    __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    for (size_t i = 0; i < list.current_size; ++i) {
        ((uint8_t *)list.data)[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(n <= list.length);

    struct aws_array_list old;
    size_t old_capacity;

    __CPROVER_assume(
        list.current_size == 0 ||
        __CPROVER_separate(list.data, &list, &n, &old, &old_capacity));

    old = list;
    old_capacity = aws_array_list_capacity(&old);

    aws_array_list_pop_front_n(&list, n);

    assert(list.length == old.length - n);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(aws_array_list_is_valid(&list));
}
