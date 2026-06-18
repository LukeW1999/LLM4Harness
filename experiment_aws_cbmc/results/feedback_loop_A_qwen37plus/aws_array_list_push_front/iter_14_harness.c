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
    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity > 0 && initial_capacity < 100);
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size < 100);
    
    int init_result = aws_array_list_init_dynamic(&list, aws_default_allocator(), initial_capacity, item_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length < 10);
    
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    
    struct aws_array_list old = list;
    
    int result = aws_array_list_push_front(&list, val);
    
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
    } else {
        assert(list.length == old.length);
    }
    
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));
    
    aws_array_list_clean_up(&list);
    free(val);
}
