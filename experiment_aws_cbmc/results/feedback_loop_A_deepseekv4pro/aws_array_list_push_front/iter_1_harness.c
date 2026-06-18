#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    /* Bound and initialize the list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    /* Create a valid input element */
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, list.item_size));

    /* Save old state for postcondition checks */
    struct aws_array_list old = list;
    size_t old_data_size = old.length * old.item_size;
    uint8_t *old_data = NULL;
    if (old_data_size > 0) {
        old_data = malloc(old_data_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_memcpy(old_data, old.data, old_data_size);
    }

    int result = aws_array_list_push_front(&list, val);

    /* Unchanged fields under all circumstances */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        /* Length increases by one */
        assert(list.length == old.length + 1);

        /* The new first element is exactly val */
        assert_bytes_match(list.data, val, list.item_size);

        /* Existing elements shifted right by one position */
        if (old.length > 0) {
            for (size_t i = 0; i < old.length; i++) {
                assert_bytes_match((uint8_t *)list.data + (i + 1) * list.item_size,
                                   old_data + i * old.item_size,
                                   old.item_size);
            }
        }
    } else {
        /* On failure the list must remain unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old_data_size > 0) {
            assert_bytes_match(list.data, old_data, old_data_size);
        }
    }

    /* Overall validity invariant must hold */
    assert(aws_array_list_is_valid(&list));

    /* Clean up */
    free(val);
    if (old_data) free(old_data);
}
