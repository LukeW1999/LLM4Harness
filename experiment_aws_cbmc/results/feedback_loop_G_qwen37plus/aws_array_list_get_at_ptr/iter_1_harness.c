#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val;
    void **val_ptr = &val;
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val_ptr, sizeof(void *)));

    size_t index = nondet_size_t();
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION);

    struct aws_array_list old = list;

    int result = aws_array_list_get_at_ptr(&list, val_ptr, index);

    if (result == AWS_OP_SUCCESS) {
        assert(*val_ptr == (void *)((uint8_t *)list.data + (list.item_size * index)));
    } else {
        assert(result == AWS_OP_ERR);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));
}
