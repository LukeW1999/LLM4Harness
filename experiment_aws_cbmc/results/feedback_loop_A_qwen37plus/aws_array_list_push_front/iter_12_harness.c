#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_push_front_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_array_list list;
    list.alloc = allocator;
    ensure_array_list_has_allocated_data(&list);
    
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    void *val = malloc(list.item_size);
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
