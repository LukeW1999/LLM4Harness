#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    struct aws_array_list old = list;

    uint8_t *old_data = malloc(old.current_size);
    __CPROVER_assume(old_data != NULL);

    uint8_t *data = (uint8_t *)list.data;
    for (size_t i = 0; i < old.current_size; ++i) {
        old_data[i] = data[i];
    }

    size_t offset_a = a * old.item_size;
    size_t offset_b = b * old.item_size;

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    uint8_t *new_data = (uint8_t *)list.data;

    if (a == b) {
        assert_bytes_match(new_data, old_data, old.current_size);
    } else {
        assert_bytes_match(new_data + offset_a, old_data + offset_b, old.item_size);
        assert_bytes_match(new_data + offset_b, old_data + offset_a, old.item_size);

        size_t byte_index = nondet_size_t();
        __CPROVER_assume(byte_index < old.current_size);

        bool byte_in_a = byte_index >= offset_a && (byte_index - offset_a) < old.item_size;
        bool byte_in_b = byte_index >= offset_b && (byte_index - offset_b) < old.item_size;

        if (!byte_in_a && !byte_in_b) {
            assert(new_data[byte_index] == old_data[byte_index]);
        }
    }

    assert(a < list.length);
    assert(b < list.length);
    assert(aws_array_list_is_valid(&list));
}
