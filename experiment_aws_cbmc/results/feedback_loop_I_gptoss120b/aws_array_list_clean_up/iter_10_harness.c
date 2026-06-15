#include <assert.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_clean_up_harness() {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* Use small, deterministic sizes to keep the verification tractable */
    size_t item_size = 8;
    size_t init_capacity = 4;

    if (aws_array_list_init(&list, alloc, init_capacity, item_size) != AWS_OP_SUCCESS) {
        return;
    }

    struct aws_array_list old = list;

    aws_array_list_clean_up(&list);

    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.alloc == NULL);
    assert(list.item_size == old.item_size);
}
