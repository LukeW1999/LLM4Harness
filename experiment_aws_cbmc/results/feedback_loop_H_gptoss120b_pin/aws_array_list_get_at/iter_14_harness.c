#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_get_at_harness(void) {
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

    void *val = NULL;
    if (list.item_size > 0) {
        val = aws_mem_acquire(allocator, list.item_size);
        __CPROVER_assume(val != NULL);
        for (size_t i = 0; i < list.item_size; ++i) {
            ((uint8_t *)val)[i] = nondet_uint8_t();
        }
    }

    int result = aws_array_list_get_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));

    /* The list structure must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    if (old.data != NULL) {
        assert_byte_from_buffer_matches(old.data, &old_data_storage);
    }

    if (result == AWS_OP_SUCCESS) {
        /* Successful get requires index within bounds */
        assert(index < old.length);
        if (list.data != NULL && val != NULL) {
            assert_bytes_match(
                (uint8_t *)list.data + (index * list.item_size),
                (uint8_t *)val,
                list.item_size);
        }
    } else {
        /* Failure occurs when index is out of bounds */
        assert(index >= old.length);
    }

    if (val != NULL) {
        aws_mem_release(allocator, val);
    }
}
