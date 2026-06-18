#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
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

    size_t index = nondet_size_t();
    /* Bound index to prevent overflow/excessive allocation in ensure_capacity */
    __CPROVER_assume(index <= MAX_INITIAL_ITEM_ALLOCATION * 2);

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;

    int result = aws_array_list_set_at(&list, val, index);

    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }
        
        /* Verify the element was set correctly */
        uint8_t *ptr = (uint8_t *)list.data + (list.item_size * index);
        for (size_t i = 0; i < list.item_size; i++) {
            assert(ptr[i] == val[i]);
        }
    } else {
        /* On failure, length remains unchanged */
        assert(list.length == old.length);
    }
}
