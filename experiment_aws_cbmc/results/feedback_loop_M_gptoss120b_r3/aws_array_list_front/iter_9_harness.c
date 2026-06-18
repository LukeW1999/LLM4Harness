#include <assert.h>
#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 64

void aws_array_list_front_harness() {
    struct aws_array_list list = {0};
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *front = aws_array_list_front(&list);

    assert(aws_array_list_is_valid(&list));

    if (list.length > 0) {
        assert(front == (void *)((uint8_t *)list.data));
    } else {
        assert(front == NULL);
    }
}
