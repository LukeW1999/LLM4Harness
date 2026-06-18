#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    list.alloc = allocator;
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    unsigned char value[MAX_ITEM_SIZE + 1];

    int result = aws_array_list_front(&list, value);

    if (old.length == 0) {
        __CPROVER_assert(result == AWS_OP_ERR, "front of empty list fails");
    } else {
        __CPROVER_assert(result == AWS_OP_SUCCESS, "front of non-empty list succeeds");
        for (size_t i = 0; i < old.item_size; ++i) {
            __CPROVER_assert(value[i] == ((unsigned char *)old.data)[i], "front copies first element");
        }
    }

    __CPROVER_assert(list.alloc == old.alloc, "allocator unchanged");
    __CPROVER_assert(list.data == old.data, "data unchanged");
    __CPROVER_assert(list.current_size == old.current_size, "current_size unchanged");
    __CPROVER_assert(list.length == old.length, "length unchanged");
    __CPROVER_assert(list.item_size == old.item_size, "item_size unchanged");
    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
