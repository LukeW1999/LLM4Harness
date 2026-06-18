#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;
    uint8_t raw_array[MAX_BUFFER_SIZE];

    size_t item_count = nondet_size_t();
    size_t item_size = nondet_size_t();

    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(item_size <= MAX_BUFFER_SIZE);
    __CPROVER_assume(item_count <= MAX_BUFFER_SIZE / item_size);

    size_t current_size = item_count * item_size;

    size_t byte_index = nondet_size_t();
    __CPROVER_assume(byte_index < current_size);
    uint8_t old_byte = raw_array[byte_index];

    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    __CPROVER_assert(aws_array_list_is_valid(&list), "list is valid");
    __CPROVER_assert(list.alloc == NULL, "list allocator is null");
    __CPROVER_assert(list.current_size == current_size, "list current_size is correct");
    __CPROVER_assert(list.length == 0, "list length is zero");
    __CPROVER_assert(list.item_size == item_size, "list item_size is correct");
    __CPROVER_assert(list.data == (void *)raw_array, "list data is raw_array");

    __CPROVER_assert(aws_array_list_length(&list) == 0, "list length accessor returns zero");
    __CPROVER_assert(list.current_size / list.item_size == item_count, "list capacity is item_count");
    __CPROVER_assert(raw_array[byte_index] == old_byte, "raw array contents unchanged");
}
