#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_get_at_ptr_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t index = nondet_size_t();
    void *val = NULL;
    void *old_val = val;

    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    int result = aws_array_list_get_at_ptr(&list, &val, index);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (old.length > index) {
        assert(result == AWS_OP_SUCCESS);
        assert(val == (void *)((uint8_t *)old.data + (old.item_size * index)));
    } else {
        assert(result == AWS_OP_ERR);
        assert(val == old_val);
    }

    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    assert_array_list_equivalence(&list, &old, &old_byte);
    assert(aws_array_list_is_valid(&list));
}
