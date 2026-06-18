#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

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

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    size_t index = nondet_size_t();

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        size_t expected_length = old.length;
        if (index >= old.length) {
            expected_length = index + 1;
        }
        assert(list.length == expected_length);

        if (list.data) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size), val, list.item_size);
        }

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (list.data == old.data && list.current_size > 0) {
            size_t overwritten_start = index * list.item_size;
            size_t overwritten_end = overwritten_start + list.item_size;
            if (old_byte.offset < overwritten_start || old_byte.offset >= overwritten_end) {
                assert_byte_from_buffer_matches(list.data, &old_byte);
            }
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);

        if (list.data && list.current_size > 0) {
            assert_byte_from_buffer_matches(list.data, &old_byte);
        }
    }

    free(val);
}
