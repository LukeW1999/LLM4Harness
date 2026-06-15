#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clear_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    if (aws_array_list_init_dynamic(&list, allocator, 10, sizeof(int)) != AWS_OP_SUCCESS) {
        return 0;
    }

    aws_array_list_clear(&list);
    assert(list.length == 0);

    aws_array_list_clean_up(&list);
    return 0;
}
