#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/array_list.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    aws_array_list_init(&list, alloc, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);

    size_t len = (size_t)nondet_uint();
    __CPROVER_assume(len <= list.capacity);
    list.length = len;

    size_t old_length   = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    struct aws_allocator *old_alloc = list.alloc;
    void *old_data = list.data;

    int ret = aws_array_list_pop_front(&list);

    if (old_length > 0) {
        assert(ret == AWS_OP_SUCCESS);
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(ret == AWS_ERROR_LIST_EMPTY);
        assert(aws_array_list_length(&list) == old_length);
    }

    assert(aws_array_list_capacity(&list) == old_capacity);
    assert(list.alloc == old_alloc);
    assert(list.data == old_data);
    assert(aws_array_list_is_valid(&list));

    aws_array_list_clean_up(&list);
}
