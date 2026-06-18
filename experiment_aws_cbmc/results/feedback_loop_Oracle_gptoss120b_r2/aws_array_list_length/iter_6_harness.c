#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate and initialize the list with bounded sizes to avoid CBMC blow‑up */
    ensure_array_list_is_allocated(&list, allocator);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Impose reasonable bounds on the internal fields */
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 64);
    __CPROVER_assume(list.current_size <= 256);
    __CPROVER_assume(list.length <= (list.current_size / list.item_size));

    size_t ret_len = aws_array_list_length(&list);

    /* The length function must return the stored length and leave the list unchanged */
    assert(ret_len == list.length);
    assert(aws_array_list_is_valid(&list));
}
