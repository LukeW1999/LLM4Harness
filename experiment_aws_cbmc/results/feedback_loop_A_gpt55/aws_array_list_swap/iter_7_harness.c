#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_swap_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length > 1);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);
    __CPROVER_assume(a != b);

    struct aws_array_list old = list;

    __CPROVER_assume(a <= SIZE_MAX / old.item_size);
    __CPROVER_assume(b <= SIZE_MAX / old.item_size);

    size_t offset_a = a * old.item_size;
    size_t offset_b = b * old.item_size;

    __CPROVER_assume(old.item_size <= old.current_size);
    __CPROVER_assume(offset_a <= old.current_size - old.item_size);
    __CPROVER_assume(offset_b <= old.current_size - old.item_size);

    uint8_t *data = (uint8_t *)list.data;

    size_t item_byte_index = nondet_size_t();
    __CPROVER_assume(item_byte_index < old.item_size);

    uint8_t old_a_byte = data[offset_a + item_byte_index];
    uint8_t old_b_byte = data[offset_b + item_byte_index];

    size_t byte_index = nondet_size_t();
    __CPROVER_assume(byte_index < old.current_size);

    int byte_in_a = byte_index >= offset_a && byte_index < offset_a + old.item_size;
    int byte_in_b = byte_index >= offset_b && byte_index < offset_b + old.item_size;

    uint8_t old_unaffected_byte = data[byte_index];

    aws_array_list_swap(&list, a, b);

    __CPROVER_assert(list.alloc == old.alloc, "allocator is unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size is unchanged");
    __CPROVER_assert(list.length == old.length, "length is unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size is unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer is unchanged");

    uint8_t *new_data = (uint8_t *)list.data;

    __CPROVER_assert(new_data[offset_a + item_byte_index] == old_b_byte, "byte from b moved to a");
    __CPROVER_assert(new_data[offset_b + item_byte_index] == old_a_byte, "byte from a moved to b");

    if (!byte_in_a && !byte_in_b) {
        __CPROVER_assert(new_data[byte_index] == old_unaffected_byte, "bytes outside swapped items are unchanged");
    }

    __CPROVER_assert(a < list.length, "a remains in bounds");
    __CPROVER_assert(b < list.length, "b remains in bounds");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
