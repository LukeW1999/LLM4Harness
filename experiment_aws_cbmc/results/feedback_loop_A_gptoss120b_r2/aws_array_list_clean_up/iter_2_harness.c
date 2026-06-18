#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* Harness for aws_array_list_clean_up */
void aws_array_list_clean_up_harness() {
    /* 1. Declare and bound the array list */
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    /* The allocator must be non‑NULL for a valid list */
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* 2. Call the function under test */
    aws_array_list_clean_up(&list);

    /* 3. Post‑conditions: fields must be zero / NULL after clean‑up */
    assert(list.data == NULL);
    assert(list.current_size == 0);
    assert(list.length == 0);
    assert(list.item_size == 0);
    /* allocator is not required to be NULL after clean_up */
}
