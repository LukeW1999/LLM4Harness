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

    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION * 2);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == (index >= old.length ? index + 1 : old.length));
        assert_bytes_match((uint8_t *)list.data + index * list.item_size, val, list.item_size);
    } else {
        assert(list.length == old.length);
    }

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(aws_array_list_is_valid(&list));

    free(val);
}
