#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    int result = aws_array_list_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        assert_bytes_match((uint8_t *)list.data, val, list.item_size);
    } else {
        assert(list.length == 0);
    }

    free(val);
}
