#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bounds to keep the state space tractable */
#define MAX_ITEM_SIZE 4
#define MAX_LENGTH   2

void aws_array_list_set_at_harness() {
    struct aws_array_list list;

    /* Ensure the list has the default allocator and is valid */
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Bound sizes to keep state space tractable */
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);
    __CPROVER_assume(list.length <= MAX_LENGTH);
    __CPROVER_assume(list.current_size >= list.length * list.item_size);
    __CPROVER_assume(list.current_size <= MAX_LENGTH * MAX_ITEM_SIZE);

    /* Allocate the value to set */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* Nondeterministic index, bounded to cover both valid and invalid cases */
    size_t index = nondet_size_t();
    __CPROVER_assume(index <= MAX_LENGTH + 1); /* 0 .. MAX_LENGTH+1 ensures out-of-bounds possible */

    /* Save a copy of the old list for postcondition checking */
    struct aws_array_list old_list = list;

    /* Save the entire old data buffer content for bytewise comparison */
    uint8_t *old_data = NULL;
    if (list.data != NULL && list.current_size > 0) {
        old_data = (uint8_t *)malloc(list.current_size);
        __CPROVER_assume(old_data != NULL);
        for (size_t i = 0; i < list.current_size; i++) {
            old_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    int result = aws_array_list_set_at(&list, val, index);

    /* Postconditions */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* Check that index was within bounds */
        assert(index < old_list.length);

        /* The element at index now holds val */
        assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                           val, list.item_size);

        /* List dimensions unchanged */
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* All bytes outside the modified element remain unchanged */
        if (old_data != NULL) {
            size_t modified_start = index * list.item_size;
            size_t modified_end = modified_start + list.item_size;
            for (size_t i = 0; i < old_list.current_size; i++) {
                if (i >= modified_start && i < modified_end) continue;
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    } else {
        /* On error, the list must be completely unchanged */
        assert(list.data == old_list.data);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);

        /* Data content unchanged */
        if (old_data != NULL) {
            for (size_t i = 0; i < old_list.current_size; i++) {
                assert(((uint8_t *)list.data)[i] == old_data[i]);
            }
        }
    }

    free(val);
    if (old_data != NULL) free(old_data);
}
