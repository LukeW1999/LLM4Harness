#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

__CPROVER_bool nondet_bool(void);

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;

    if (aws_array_list_init(&list, allocator, 10, sizeof(int)) != AWS_OP_SUCCESS) {
        return;
    }

    __CPROVER_bool do_push = nondet_bool();
    if (do_push) {
        int val;
        if (aws_array_list_push_back(&list, &val) != AWS_OP_SUCCESS) {
            aws_array_list_clean_up(&list);
            return;
        }
    }

    void *front = aws_array_list_front(&list);

    if (do_push) {
        assert(front != NULL);
    } else {
        assert(front == NULL);
    }

    aws_array_list_clean_up(&list);
}
