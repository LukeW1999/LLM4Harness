#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    size_t initial_capacity = nondet_size_t();
    __CPROVER_assume(initial_capacity > 0 && initial_capacity < 100);
    
    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0 && item_size < 100);
    
    struct aws_array_list list;
    int init_result = aws_array_list_init_dynamic(&list, aws_default_allocator(), initial_capacity, item_size);
    
    if (init_result != AWS_OP_SUCCESS) {
        return;
    }
    
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    size_t target_length = nondet_size_t();
    __CPROVER_assume(target_length <= list.current_size);
    list.length = target_length;
    
    void *val = malloc(item_size);
    if (val == NULL) {
        aws_array_list_clean_up(&list);
        return;
    }
    
    size_t old_length = list.length;
    void *old_data = list.data;
    size_t old_current_size = list.current_size;
    struct aws_allocator *old_alloc = list.alloc;
    size_t old_item_size = list.item_size;
    
    int result = aws_array_list_push_front(&list, val);
    
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
    } else {
        assert(list.length == old_length);
    }
    
    assert(list.alloc == old_alloc);
    assert(list.item_size == old_item_size);
    assert(aws_array_list_is_valid(&list));
    
    aws_array_list_clean_up(&list);
    free(val);
}
