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
    if (aws_array_list_is_bounded(&list)) {
        ensure_array_list_has_allocated_data(&list);
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
}
