#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare the array list and bound its fields */
    struct aws_array_list list;

    /* allocator must be a real allocator for the pre‑condition of validity */
    list.alloc = aws_default_allocator();

    /* nondet item size bounded */
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0 && list.item_size <= MAX_ITEM_SIZE);

    /* nondet current_size and length bounded by the maximum initial allocation */
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= MAX_INITIAL_ITEM_ALLOCATION);

    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);

    /* allocate the data buffer if the list is supposed to own memory */
    ensure_array_list_has_allocated_data_member(&list);

    /* bound the whole structure */
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));

    /* pre‑condition: the list must be valid before clean‑up */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state for immutability checks (not needed for this function,
       but kept for completeness) */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Post‑conditions: all fields are zeroed */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);

    /* 5. The list is no longer a valid aws_array_list after clean‑up */
    assert(!aws_array_list_is_valid(&list));
}
