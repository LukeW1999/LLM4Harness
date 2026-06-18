#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    
    struct aws_array_list old = list;
    
    int result = aws_array_list_set_at(&list, val, index);
    
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    
    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        
        uint8_t *dest = (uint8_t *)list.data + (list.item_size * index);
        assert_bytes_match(dest, val, list.item_size);
    } else {
        assert(list.length == old.length);
    }
}
