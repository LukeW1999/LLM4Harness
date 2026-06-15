#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_erase_harness(void) {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct store_byte_from_buffer data_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((uint8_t *)list.data, list.current_size, &data_byte);
    }

    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    bool will_succeed = nondet_bool();

    if (will_succeed) {
        __CPROVER_assume(list.length > 0);
        __CPROVER_assume(index < list.length);
    } else {
        __CPROVER_assume(list.length == 0 || index >= list.length);
    }

    int result = aws_array_list_erase(&list, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length - 1);
    } else {
        assert(list.length == old.length);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);
    assert(list.current_size == old.current_size);

    assert(aws_array_list_is_valid(&list));
}
