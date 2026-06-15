#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 8

void aws_array_list_pop_back_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    aws_array_list_init(&list, allocator, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE);
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    uint8_t out_buf[MAX_ITEM_SIZE];
    void *out = out_buf;

    int result = aws_array_list_pop_back(&list, out);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
    } else {
        assert(list.length == old.length);
    }
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
