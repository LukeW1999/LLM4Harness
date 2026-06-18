#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_ITEM_SIZE ((size_t)100)
#define MAX_INITIAL_SIZE ((size_t)10)

void harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    size_t initial_size;
    size_t item_size;
    __CPROVER_assume(initial_size <= MAX_INITIAL_SIZE);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);
    aws_array_list_init_dynamic(&list, allocator, initial_size, item_size);

    size_t len;
    int result = aws_array_list_length(&list, &len);

    assert(result == AWS_OP_SUCCESS);
    assert(len == list.length);
    assert(aws_array_list_is_valid(&list));
}
