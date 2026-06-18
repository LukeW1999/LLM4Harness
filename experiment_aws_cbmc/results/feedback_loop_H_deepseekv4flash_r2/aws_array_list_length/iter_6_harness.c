#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 100

void aws_array_list_length_harness() {
    struct aws_array_list list;
    size_t item_size;
    size_t length;
    size_t current_size;
    void *data;

    item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    length = nondet_size_t();
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    current_size = length * item_size;
    data = malloc(current_size);
    __CPROVER_assume(data != NULL);

    list.alloc = aws_default_allocator();
    list.item_size = item_size;
    list.length = length;
    list.current_size = current_size;
    list.data = data;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t len;
    int result = aws_array_list_length(&list, &len);

    assert(result == AWS_OP_SUCCESS);
    assert(len == list.length);
    assert(aws_array_list_is_valid(&list));

    free(data);
}
