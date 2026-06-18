#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val;
    int ret = aws_array_list_front(&list, &val);
    if (aws_array_list_length(&list) > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(val == list.data);
        assert(AWS_MEM_IS_READABLE(val, list.item_size));
    } else {
        assert(ret == AWS_ERROR_LIST_EMPTY);
    }
    assert(aws_array_list_is_valid(&list));
}
