#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(
        &list,
        MAX_INITIAL_ITEM_ALLOCATION,
        MAX_ITEM_SIZE));

    /* Allocate the internal data buffer if needed */
    ensure_array_list_has_allocated_data_member(&list);

    /* Use a deterministic allocator (default) */
    list.alloc = aws_default_allocator();

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 4. Post‑conditions: all fields must be zero after clean‑up */
    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);
    assert(list.item_size == 0);

    /* No validity assertion after clean_up, as a zeroed list is not considered valid */
}
