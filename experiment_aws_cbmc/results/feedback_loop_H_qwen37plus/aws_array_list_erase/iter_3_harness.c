#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t index = nondet_size_t();

    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_length(&list) + 1 == old.length);
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
