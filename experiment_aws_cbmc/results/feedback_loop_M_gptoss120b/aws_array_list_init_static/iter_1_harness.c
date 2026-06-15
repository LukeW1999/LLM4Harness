#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_init_static_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t item_count = nondet_size_t();
    __CPROVER_assume(item_count > 0);
    __CPROVER_assume(item_count <= MAX_INITIAL_ITEM_ALLOCATION);

    size_t item_size = nondet_size_t();
    __CPROVER_assume(item_size > 0);
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* Prevent multiplication overflow */
    __CPROVER_assume(item_count <= SIZE_MAX / item_size);
    size_t total_size = item_count * item_size;

    /* 2. Allocate the raw array that will back the list */
    void *raw_array = malloc(total_size);
    __CPROVER_assume(raw_array != NULL);

    /* Save a snapshot of the raw array contents (they must stay unchanged) */
    struct store_byte_from_buffer raw_array_storage;
    save_byte_from_array((uint8_t *)raw_array, total_size, &raw_array_storage);

    /* 3. Declare the list structure and bound it */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* Save the old state of the list (for immutability checks) */
    struct aws_array_list old = list;

    /* 4. Call the function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* 5. Post‑condition assertions */

    /* Fields that must be set by the function */
    assert(list.alloc == NULL);
    assert(list.item_size == item_size);
    assert(list.length == 0);
    assert(list.data == raw_array);
    assert(list.current_size == total_size);

    /* Fields that must remain unchanged (none other than those above) */
    /* The struct only contains the fields we have already asserted. */

    /* Raw backing array must be unchanged */
    assert_byte_from_buffer_matches((uint8_t *)raw_array, &raw_array_storage);

    /* The list must satisfy its validity invariant */
    assert(aws_array_list_is_valid(&list));
}
