#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length <= list.capacity);
    list.alloc = allocator;

    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_data_byte;
    if (old.data != NULL && old.length > 0) {
        save_byte_from_array(old.data,
                             old.length * old.item_size,
                             &old_data_byte);
    }

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_bytes_match((uint8_t *)list.data,
                           (uint8_t *)val,
                           list.item_size);
        if (old.length > 0) {
            assert_bytes_match((uint8_t *)list.data + list.item_size,
                               (uint8_t *)old.data,
                               old.length * old.item_size);
        }
    } else {
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        if (old.data != NULL && old.length > 0) {
            assert_byte_from_buffer_matches(list.data,
                                            &old_data_byte);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
}
