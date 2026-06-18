#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>
#include <stdlib.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    list.alloc = allocator;

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_storage = {0};
    if (old.data != NULL) {
        save_byte_from_array(old.data, old.current_size, &old_data_storage);
    }

    size_t index = nondet_size_t();

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            val[i] = nondet_uint8_t();
        }
    }

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        if (list.data != NULL) {
            assert_bytes_match((uint8_t *)list.data + (index * list.item_size),
                               val,
                               list.item_size);
        }
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old.data != NULL) {
            assert_byte_from_buffer_matches(old.data, &old_data_storage);
        }
    }

    free(val);
}
