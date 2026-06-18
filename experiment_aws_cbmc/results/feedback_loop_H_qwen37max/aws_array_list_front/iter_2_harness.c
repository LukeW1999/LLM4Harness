#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_front_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = (uint8_t *)malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;

    int result = aws_array_list_front(&list, val);

    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
    } else {
        assert(list.length == 0);
    }

    free(val);
}
