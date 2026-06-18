#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_pop_front_n_harness() {
    struct aws_array_list list;
    size_t n = nondet_size_t();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.length <= list.current_size / list.item_size);
    __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    struct aws_array_list old = list;

    uint8_t *old_data = malloc(old.current_size);
    __CPROVER_assume(old.current_size == 0 || old_data != NULL);

    uint8_t *list_data = (uint8_t *)list.data;
    for (size_t i = 0; i < old.current_size; ++i) {
        old_data[i] = list_data[i];
    }

    aws_array_list_pop_front_n(&list, n);

    assert(aws_array_list_is_valid(&list));

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == aws_array_list_capacity(&old));

    if (n >= old.length) {
        assert(list.length == 0);
    } else {
        size_t remaining_items = old.length - n;
        size_t popping_bytes = old.item_size * n;
        size_t remaining_bytes = remaining_items * old.item_size;

        assert(list.length == remaining_items);

        uint8_t *new_data = (uint8_t *)list.data;
        for (size_t i = 0; i < remaining_bytes; ++i) {
            assert(new_data[i] == old_data[i + popping_bytes]);
        }
    }
}
