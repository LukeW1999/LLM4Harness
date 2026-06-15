#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdbool.h>

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(
        aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);

    /* allocator may be NULL or a valid allocator */
    list.alloc = nondet_bool() ? aws_default_allocator() : NULL;

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    aws_array_list_clean_up(&list);

    /* post‑conditions */
    assert(list.data == NULL);
    assert(list.length == 0);
    assert(list.current_size == 0);

    /* allocator and item_size may remain unchanged */
    assert(aws_array_list_is_valid(&list));
}
