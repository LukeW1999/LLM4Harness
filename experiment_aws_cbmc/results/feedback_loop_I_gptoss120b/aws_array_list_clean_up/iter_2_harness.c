#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound the data structure */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Save old state */
    struct aws_array_list old = list;

    /* 3. Call function under test */
    aws_array_list_clean_up(&list);

    /* 4. Post‑conditions: fields that clean_up zeroes */
    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);

    /* allocator and item_size are unchanged by clean_up */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    /* 5. Validity invariant must still hold */
    assert(aws_array_list_is_valid(&list));
}
