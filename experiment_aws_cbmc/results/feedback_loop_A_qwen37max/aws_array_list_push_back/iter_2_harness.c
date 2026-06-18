#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 10
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 10
#endif

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index;
    
    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;
    
    int result = aws_array_list_set_at(&list, val, index);

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
    }
    
    free(val);
}
