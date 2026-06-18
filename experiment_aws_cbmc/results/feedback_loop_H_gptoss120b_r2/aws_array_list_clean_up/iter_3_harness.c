#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));

    /* Allocate the internal data buffer (if any) */
    ensure_array_list_has_allocated_data_member(&list);

    /* Use the default allocator (cannot be NULL for clean_up) */
    list.alloc = aws_default_allocator();

    /* Assume the list is initially valid */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 3. Post‑conditions: data and size fields must be zero after clean‑up */
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    /* item_size may remain unchanged; it is not required to be zero */

    /* 4. The list must remain valid (a zeroed list is a valid empty list) */
    assert(aws_array_list_is_valid(&list));
}
