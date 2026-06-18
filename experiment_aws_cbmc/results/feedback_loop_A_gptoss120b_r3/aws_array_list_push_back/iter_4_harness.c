#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t item_sz = list.item_size ? list.item_size : 1;
    struct aws_allocator *allocator = aws_default_allocator();
    uint8_t *val = (uint8_t *)aws_mem_acquire(allocator, item_sz);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_sz));

    size_t index = nondet_size_t();

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte = {0};
    if (list.data) {
        save_byte_from_array(list.data, list.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        size_t offset = index * list.item_size;
        assert(AWS_MEM_IS_WRITABLE((uint8_t *)list.data + offset, list.item_size));
        assert_bytes_match((uint8_t *)list.data + offset,
                           (const uint8_t *)val,
                           list.item_size);

        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }

        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size >= old.current_size);
    } else {
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.data == old.data);
        if (list.data) {
            assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));
}
