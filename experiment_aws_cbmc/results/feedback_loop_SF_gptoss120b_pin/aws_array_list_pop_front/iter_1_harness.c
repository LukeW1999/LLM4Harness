#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_pop_front_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic initialization of the list */
    ensure_array_list_is_valid(&list, alloc);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot of pre‑call state */
    size_t old_length = aws_array_list_length(&list);
    size_t old_capacity = aws_array_list_capacity(&list);
    void *old_data = list.data;
    size_t old_item_size = list.item_size;

    /* call under verification */
    int result = aws_array_list_pop_front(&list);

    /* post‑condition assertions */
    /* result must be either success (0) or error (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* if the list was empty, pop_front must fail; otherwise it must succeed */
    if (old_length == 0) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
    }

    /* the list must remain valid */
    assert(aws_array_list_is_valid(&list));

    /* allocator, item size, data pointer, and capacity are unchanged */
    assert(list.alloc == alloc);
    assert(list.item_size == old_item_size);
    assert(list.data == old_data);
    assert(aws_array_list_capacity(&list) == old_capacity);

    /* length updates correctly */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_length(&list) == old_length - 1);
    } else {
        assert(aws_array_list_length(&list) == old_length);
    }
}
