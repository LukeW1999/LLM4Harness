#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    size_t item_size = nondet_size_t();
    size_t initial_item_allocation = nondet_size_t();

    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(initial_item_allocation <= MAX_INITIAL_ITEM_ALLOCATION);

    if (aws_array_list_init_dynamic(&list, aws_default_allocator(), initial_item_allocation, item_size) != AWS_OP_SUCCESS) {
        return;
    }
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.current_size >= old.current_size);
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(aws_array_list_is_valid(&list));
    }

    free(val);
    aws_array_list_clean_up(&list);
}
