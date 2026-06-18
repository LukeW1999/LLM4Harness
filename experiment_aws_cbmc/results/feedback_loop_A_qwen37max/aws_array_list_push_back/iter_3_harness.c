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

void aws_array_list_push_back_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t val[MAX_ITEM_SIZE];

    struct aws_array_list old = list;
    
    int result = aws_array_list_push_back(&list, val);

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.length == old.length + 1);
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.length == old.length);
    }
}
