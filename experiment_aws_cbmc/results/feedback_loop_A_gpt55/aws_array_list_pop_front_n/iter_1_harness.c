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
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    for (size_t i = 0; i < list.current_size; ++i) {
        old_data[i] = ((const uint8_t *)list.data)[i];
    }

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);

    aws_array_list_pop_front_n(&list, n);

    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        size_t remaining_items = old.length - n;
        size_t popping_bytes = n * old.item_size;
        size_t remaining_bytes = remaining_items * old.item_size;

        assert(list.length == remaining_items);
        assert_bytes_match((const uint8_t *)list.data, old_data + popping_bytes, remaining_bytes);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(aws_array_list_is_valid(&list));
}
