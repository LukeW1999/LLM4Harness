#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size <= 100);
    
    size_t initial_item_allocation = nondet_size_t();
    __CPROVER_assume(initial_item_allocation <= 10);
    
    int init_result = aws_array_list_init(&list, allocator, initial_item_allocation, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    struct aws_array_list old = list;
    
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    
    int result = aws_array_list_push_front(&list, val);
    
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    aws_array_list_clean_up(&list);
}
