#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (list.data && list.current_size > 0) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    uint8_t *val = allocator->mem_acquire(allocator, list.item_size);
    __CPROVER_assume(val != NULL);
    size_t index = nondet_size_t();

    int result = aws_array_list_set_at(&list, val, index);

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        __CPROVER_assert(list.length == expected_length, "length updated correctly");

        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size), val, list.item_size);
        }

        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");

        if (list.data == old.data && list.current_size > 0) {
            size_t overwritten_start = index * list.item_size;
            size_t overwritten_end = overwritten_start + list.item_size;
            if (old_byte.offset < overwritten_start || old_byte.offset >= overwritten_end) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        }
    } else {
        __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged on failure");
        __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged on failure");
        __CPROVER_assert(list.length == old.length, "length unchanged on failure");
        __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged on failure");
        __CPROVER_assert(list.data == old.data, "data unchanged on failure");

        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    allocator->mem_release(allocator, val);
}
