#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t front_value[MAX_ITEM_SIZE + 1];

    int rval = aws_array_list_front(&list, front_value);

    if (list.length == 0) {
        __CPROVER_assert(rval == AWS_OP_ERR, "front of an empty list returns AWS_OP_ERR");
    } else {
        __CPROVER_assert(rval == AWS_OP_SUCCESS, "front of a non-empty list returns AWS_OP_SUCCESS");

        for (size_t i = 0; i < list.item_size; ++i) {
            __CPROVER_assert(
                front_value[i] == ((uint8_t *)list.data)[i],
                "front of a non-empty list copies the first item");
        }
    }

    __CPROVER_assert(aws_array_list_is_valid(&list), "list remains valid");
}
