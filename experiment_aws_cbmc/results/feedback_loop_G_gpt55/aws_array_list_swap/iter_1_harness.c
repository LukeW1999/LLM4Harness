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

void aws_array_list_swap_harness(void) {
    struct aws_array_list list;

    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    assert(a < list.length);
    assert(b < list.length);

    const size_t max_size_t = (size_t)-1;
    __CPROVER_assume(a <= max_size_t / list.item_size);
    __CPROVER_assume(b <= max_size_t / list.item_size);

    size_t a_offset = a * list.item_size;
    size_t b_offset = b * list.item_size;

    __CPROVER_assume(a_offset <= max_size_t - list.item_size);
    __CPROVER_assume(b_offset <= max_size_t - list.item_size);
    __CPROVER_assume(a_offset + list.item_size <= list.current_size);
    __CPROVER_assume(b_offset + list.item_size <= list.current_size);
    __CPROVER_assume(list.current_size > 0);

    __CPROVER_assume(AWS_MEM_IS_READABLE(list.data, list.current_size));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(list.data, list.current_size));

    uint8_t *old_data = malloc(list.current_size);
    __CPROVER_assume(old_data != NULL);

    uint8_t *data = (uint8_t *)list.data;
    for (size_t i = 0; i < list.current_size; ++i) {
        old_data[i] = data[i];
    }

    struct aws_array_list old = list;

    aws_array_list_swap(&list, a, b);

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    uint8_t *new_data = (uint8_t *)list.data;

    for (size_t i = 0; i < old.item_size; ++i) {
        assert(new_data[a_offset + i] == old_data[b_offset + i]);
        assert(new_data[b_offset + i] == old_data[a_offset + i]);
    }

    for (size_t i = 0; i < old.current_size; ++i) {
        bool in_a = (i >= a_offset) && (i < a_offset + old.item_size);
        bool in_b = (i >= b_offset) && (i < b_offset + old.item_size);

        if (!in_a && !in_b) {
            assert(new_data[i] == old_data[i]);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
