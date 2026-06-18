#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE ((size_t)100)
#define MAX_INITIAL_SIZE ((size_t)10)

void harness() {
    struct aws_array_list list;
    make_aws_array_list_bounded(&list, MAX_ITEM_SIZE, MAX_INITIAL_SIZE);

    size_t len;
    int result = aws_array_list_length(&list, &len);

    assert(result == AWS_OP_SUCCESS);
    assert(len == list.length);
    assert(aws_array_list_is_valid(&list));
}
