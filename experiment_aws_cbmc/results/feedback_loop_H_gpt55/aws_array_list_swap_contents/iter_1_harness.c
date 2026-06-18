#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_contents_harness() {
    struct aws_array_list list_a;
    struct aws_array_list list_b;

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    list_a.alloc = allocator;
    list_b.alloc = allocator;
    list_a.item_size = item_size;
    list_b.item_size = item_size;

    __CPROVER_assume(aws_array_list_is_bounded(&list_a, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_bounded(&list_b, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    ensure_array_list_has_allocated_data_member(&list_a);
    ensure_array_list_has_allocated_data_member(&list_b);

    __CPROVER_assume(aws_array_list_is_valid(&list_a));
    __CPROVER_assume(aws_array_list_is_valid(&list_b));

    __CPROVER_assume(list_a.alloc != NULL);
    __CPROVER_assume(list_a.alloc == list_b.alloc);
    __CPROVER_assume(list_a.item_size == list_b.item_size);
    __CPROVER_assume(&list_a != &list_b);

    struct aws_array_list old_a = list_a;
    struct aws_array_list old_b = list_b;

    struct store_byte_from_buffer old_a_byte;
    struct store_byte_from_buffer old_b_byte;

    if (old_a.current_size > 0) {
        save_byte_from_array((const uint8_t *)old_a.data, old_a.current_size, &old_a_byte);
    }
    if (old_b.current_size > 0) {
        save_byte_from_array((const uint8_t *)old_b.data, old_b.current_size, &old_b_byte);
    }

    aws_array_list_swap_contents(&list_a, &list_b);

    assert(list_a.alloc == old_b.alloc);
    assert(list_a.current_size == old_b.current_size);
    assert(list_a.length == old_b.length);
    assert(list_a.item_size == old_b.item_size);
    assert(list_a.data == old_b.data);

    assert(list_b.alloc == old_a.alloc);
    assert(list_b.current_size == old_a.current_size);
    assert(list_b.length == old_a.length);
    assert(list_b.item_size == old_a.item_size);
    assert(list_b.data == old_a.data);

    assert(list_a.alloc == list_b.alloc);
    assert(list_a.item_size == list_b.item_size);

    if (old_a.current_size > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)list_b.data, &old_a_byte);
    }
    if (old_b.current_size > 0) {
        assert_byte_from_buffer_matches((const uint8_t *)list_a.data, &old_b_byte);
    }

    assert(aws_array_list_capacity(&list_a) == aws_array_list_capacity(&old_b));
    assert(aws_array_list_capacity(&list_b) == aws_array_list_capacity(&old_a));
    assert(aws_array_list_length(&list_a) == aws_array_list_length(&old_b));
    assert(aws_array_list_length(&list_b) == aws_array_list_length(&old_a));

    assert(aws_array_list_is_valid(&list_a));
    assert(aws_array_list_is_valid(&list_b));
}
