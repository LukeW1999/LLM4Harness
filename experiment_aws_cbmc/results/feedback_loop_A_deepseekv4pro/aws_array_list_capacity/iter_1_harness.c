#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    /* Non-deterministic bounded list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Non-deterministic value buffer */
    size_t item_size = list.item_size;
    /* item_size > 0 due to validity */
    void *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, item_size));

    /* Non-deterministic index */
    size_t index = nondet_size_t();

    /* Save old state */
    struct aws_array_list old = list;
    size_t old_length = aws_array_list_length(&list);

    /* Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* Validity invariant holds for both success and failure */
    assert(aws_array_list_is_valid(&list));

    /* Fields that must never change */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* After success, index must be within bounds */
        assert(index < aws_array_list_length(&list));

        /* The element at index must equal val */
        const uint8_t *element = (const uint8_t *)list.data + index * list.item_size;
        assert_bytes_match(element, val, list.item_size);

        /* Length updated correctly */
        if (index >= old_length) {
            assert(aws_array_list_length(&list) == index + 1);
        } else {
            assert(aws_array_list_length(&list) == old_length);
        }
    } else {
        /* On error (including overflow in length update), list length remains unchanged */
        assert(aws_array_list_length(&list) == old_length);
    }

    free(val);
}
