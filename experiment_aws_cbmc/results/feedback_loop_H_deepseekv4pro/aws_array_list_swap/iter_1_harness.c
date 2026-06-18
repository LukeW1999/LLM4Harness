#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_swap_harness() {
    struct aws_array_list list;
    
    /* Bound the list to keep CBMC tractable */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    
    /* Allocate a valid data buffer for the list */
    ensure_array_list_has_allocated_data_member(&list);
    
    /* Require the list to be valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Choose two indices within the list */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a < list.length);
    __CPROVER_assume(b < list.length);

    /* Save the old state of the list metadata */
    struct aws_array_list old = list;

    /* Compute sizes and offsets for element swapping */
    size_t data_size = list.current_size;
    size_t item_size = list.item_size;
    __CPROVER_assume(item_size > 0);  /* Valid list implies positive item_size */
    size_t offset_a = a * item_size;
    size_t offset_b = b * item_size;

    /* Maximum possible data buffer size based on bounds */
    const size_t max_bytes = MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE;

    /* Capture the entire old data content */
    uint8_t old_data[max_bytes];
    for (size_t i = 0; i < max_bytes; i++) {
        if (i < data_size) {
            old_data[i] = ((uint8_t *)list.data)[i];
        }
    }

    /* Prepare expected data after the swap */
    uint8_t expected[max_bytes];
    /* Copy old_data into expected */
    for (size_t i = 0; i < max_bytes; i++) {
        if (i < data_size) {
            expected[i] = old_data[i];
        }
    }
    /* Swap the two element blocks in the expected buffer */
    for (size_t i = 0; i < item_size; i++) {
        if (offset_a + i < data_size && offset_b + i < data_size) {
            uint8_t tmp = expected[offset_a + i];
            expected[offset_a + i] = expected[offset_b + i];
            expected[offset_b + i] = tmp;
        }
    }

    /* Perform the actual swap */
    aws_array_list_swap(&list, a, b);

    /* 1. Metadata must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    /* 2. The list must still be valid */
    assert(aws_array_list_is_valid(&list));

    /* 3. Data content must reflect the swap */
    assert_bytes_match((const uint8_t *)list.data, expected, data_size);
}
