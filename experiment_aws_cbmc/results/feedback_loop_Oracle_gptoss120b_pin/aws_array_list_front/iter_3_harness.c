#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    /* Initialize the list with capacity for 10 elements of type int */
    if (aws_array_list_init(&list, allocator, 10, sizeof(int)) != AWS_OP_SUCCESS) {
        return;
    }

    size_t len;
    __CPROVER_assume(len <= 10);

    for (size_t i = 0; i < len; ++i) {
        int val;
        /* val is nondeterministic */
        if (aws_array_list_push_back(&list, &val) != AWS_OP_SUCCESS) {
            return;
        }
    }

    void *front = aws_array_list_front(&list);

    if (len == 0) {
        assert(front == NULL);
    } else {
        assert(front != NULL);
    }

    aws_array_list_clean_up(&list);
}
