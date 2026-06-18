#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val_storage;
    void **val = &val_storage;

    size_t index = nondet_size_t();

    struct aws_array_list old_list = list;
    void *old_val = val_storage;

    int result = aws_array_list_get_at_ptr(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(index < list.length);
        assert(*val == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        assert(index >= list.length);
        assert(*val == old_val);
    }

    assert(list.alloc == old_list.alloc);
    assert(list.current_size == old_list.current_size);
    assert(list.length == old_list.length);
    assert(list.item_size == old_list.item_size);
    assert(list.data == old_list.data);
    
    assert(aws_array_list_is_valid(&list));
}
