#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

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
        __CPROVER_assert(front != NULL, "front should be non‑NULL when length > 0");
        assert_bytes_match((uint8_t *)list.data,
                           (const uint8_t *)front,
                           list.item_size);
    } else {
        __CPROVER_assert(front == NULL, "front should be NULL when length == 0");
    }

    /* Ensure the list structure itself has not been modified */
    __CPROVER_assert(list.alloc == old.alloc, "alloc unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged");
    __CPROVER_assert(list.data == old.data, "data pointer unchanged");
    assert_array_list_equivalence(&list, &old, &old_byte);

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
