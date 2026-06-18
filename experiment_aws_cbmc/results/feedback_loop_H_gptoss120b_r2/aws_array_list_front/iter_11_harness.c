#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                              MAX_INITIAL_ITEM_ALLOCATION,
                                              MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    if (old.data != NULL && old.current_size > 0) {
        save_byte_from_array(old.data, old.current_size, &old_byte);
    }

    void *front = aws_array_list_front(&list);

    if (list.length > 0) {
        assert(front != NULL);
        assert_bytes_match((uint8_t *)list.data,
                           (const uint8_t *)front,
                           list.item_size);
    } else {
        assert(front == NULL);
    }

    /* Ensure the list structure itself has not been modified */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);
    assert_array_list_equivalence(&list, &old, &old_byte);

    assert(aws_array_list_is_valid(&list));
}
