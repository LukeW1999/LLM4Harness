#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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

    struct aws_array_list old = list;
    size_t old_capacity = aws_array_list_capacity(&old);
    size_t old_length = aws_array_list_length(&old);

    uint8_t *old_data = NULL;
    if (old.current_size > 0) {
        old_data = malloc(old.current_size);
        __CPROVER_assume(old_data != NULL);
        __builtin_memcpy(old_data, old.data, old.current_size);
    }

    aws_array_list_pop_front_n(&list, n);

    if (n >= old_length) {
        assert(list.length == 0);
    } else {
        size_t remaining_items = old_length - n;
        size_t popping_bytes = old.item_size * n;
        size_t remaining_bytes = remaining_items * old.item_size;

        assert(list.length == remaining_items);

        if (n == 0) {
            if (old.length * old.item_size > 0) {
                assert_bytes_match((const uint8_t *)list.data, old_data, old.length * old.item_size);
            }
        } else {
            if (remaining_bytes > 0) {
                assert_bytes_match((const uint8_t *)list.data, old_data + popping_bytes, remaining_bytes);
            }
        }
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(aws_array_list_capacity(&list) == old_capacity);

    assert(aws_array_list_is_valid(&list));
}
