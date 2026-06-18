#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bounds to keep the state space tractable */
#define MAX_ITEM_SIZE      4
#define MAX_ITEM_ALLOCATION 2

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    /* Assume valid array list */
    __CPROVER_assume(aws_array_list_is_valid(&list));
    /* Bound the item size to avoid overflow in index arithmetic */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= MAX_ITEM_SIZE);
    /* Bound the capacity to a small number */
    __CPROVER_assume(list.current_size / list.item_size <= MAX_ITEM_ALLOCATION);

    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    size_t index = nondet_size_t();
    /* Allow index to be slightly beyond capacity to cover failure cases */
    __CPROVER_assume(index < 2 * MAX_ITEM_ALLOCATION);

    struct aws_array_list old_list = list;
    /* Save a byte from the buffer to check that unchanged parts stay unchanged */
    struct store_byte_from_buffer old_byte;
    if (list.data != NULL && list.current_size > 0) {
        save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);
    }

    int result = aws_array_list_set_at(&list, val, index);

    /* The array list must remain valid after the call */
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        /* The element at index must contain val */
        assert_bytes_match((const uint8_t *)list.data + index * list.item_size,
                           val, list.item_size);
        /* Length update: if index >= old length, length becomes index + 1,
           otherwise unchanged */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }
        /* Capacity remains unchanged */
        assert(list.current_size == old_list.current_size);
        /* Rest of the data (apart from the changed element) is unchanged */
        assert_array_list_equivalence(&list, &old_list, &old_byte);
    } else {
        /* Failure: the list must be completely unchanged */
        assert(list.data == old_list.data);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        if (old_list.data != NULL && old_list.current_size > 0) {
            assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
        }
    }

    free(val);
}
