#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE 100
#define MAX_INITIAL_SIZE 10

void harness() {
    struct aws_array_list list;
    size_t item_size;
    size_t initial_item_allocation;

    make_aws_array_list_bounded(&list, &item_size, &initial_item_allocation, MAX_ITEM_SIZE, MAX_INITIAL_SIZE);

    size_t len;
    int result = aws_array_list_length(&list, &len);

    assert(result == AWS_OP_SUCCESS);
    assert(len == list.length);
    assert(aws_array_list_is_valid(&list));
}
