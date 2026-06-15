#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    size_t index = nondet_size_t();
    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length >= index);
        assert(list.data != NULL);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    if (index < list.length) {
        assert(list.data[index] == val);
    } else {
        assert(list.length == old.length);
    }
}
