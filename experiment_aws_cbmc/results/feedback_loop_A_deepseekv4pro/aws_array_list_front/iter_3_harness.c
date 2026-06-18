#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(aws_array_list_length(&list) > 0);

    size_t item_size = list.item_size;
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    uint8_t old_front[MAX_ITEM_SIZE];
    memcpy(old_front, (uint8_t *)list.data, item_size);

    uint8_t val[MAX_ITEM_SIZE];

    int ret = aws_array_list_front(&list, val);

    assert(aws_array_list_is_valid(&list));
    assert(ret == AWS_OP_SUCCESS);
    assert(memcmp(val, old_front, item_size) == 0);
}
