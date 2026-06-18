#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    if (nondet_bool()) {
        list.alloc = aws_default_allocator();
    } else {
        list.alloc = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION);

    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.data != NULL);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);

        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        if (old.alloc == NULL) {
            assert(list.data == old.data);
            assert(list.current_size == old.current_size);
        } else {
            assert(list.current_size >= old.current_size);
        }

        assert_bytes_match((const uint8_t *)list.data + (list.item_size * index), val, list.item_size);
    } else {
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
        assert_array_list_equivalence(&list, &old, &old_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
