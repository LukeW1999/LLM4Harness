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
    /* 1. Declare and bound data structures */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state BEFORE calling */
    struct aws_array_list old = list;
    size_t data_size = list.current_size;
    uint8_t *old_data = malloc(data_size);
    if (old_data != NULL) {
        memcpy(old_data, list.data, data_size);
    }

    /* 3. Non-deterministic inputs */
    size_t index = nondet_size_t();
    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, list.item_size));

    /* 4. Call function under test */
    int result = aws_array_list_set_at(&list, val, index);

    /* 5. Assert postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Success: element at index is set to val */
        assert(list.data != NULL);
        assert(index * list.item_size + list.item_size <= list.current_size);
        assert_bytes_match((uint8_t *)list.data + index * list.item_size, val, list.item_size);

        /* Length update */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Other elements unchanged */
        for (size_t i = 0; i < old.length; i++) {
            if (i != index) {
                assert_bytes_match((uint8_t *)list.data + i * list.item_size,
                                   old_data + i * list.item_size,
                                   list.item_size);
            }
        }

        /* Fields that must not change */
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
    } else {
        /* Failure: list unchanged */
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert_bytes_match(list.data, old_data, data_size);
    }

    /* Cleanup */
    free(old_data);
    free(val);
}
