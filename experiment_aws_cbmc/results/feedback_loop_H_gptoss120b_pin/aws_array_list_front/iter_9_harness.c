#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_array_list list = {0};
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct store_byte_from_buffer old_byte;
    if (list.data && list.length > 0) {
        save_byte_from_array((uint8_t *)list.data, list.item_size, &old_byte);
    }

    const void *front = aws_array_list_front(&list);

    if (list.length > 0) {
        __CPROVER_assert(front != NULL, "front not null when list non-empty");
        assert_bytes_match((const uint8_t *)front,
                           (const uint8_t *)list.data,
                           list.item_size);
    } else {
        __CPROVER_assert(front == NULL, "front null when list empty");
    }

    __CPROVER_assert(list.alloc == allocator, "alloc unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");

    if (list.data && list.length > 0) {
        assert_byte_from_buffer_matches((uint8_t *)list.data, &old_byte);
    }
}
