#include <aws/common/array_list.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_array_list_front_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_array_list list;
    list.alloc = allocator;
    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.item_size > 0);

    struct aws_array_list old = list;

    uint8_t *out = malloc(list.item_size);
    __CPROVER_assume(out != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(out, list.item_size));

    int result = aws_array_list_front(&list, out);

    if (result == AWS_OP_SUCCESS) {
        /* The list must contain at least one element */
        __CPROVER_assert(list.length > 0, "list length > 0 on success");
        /* Verify that the front element was copied correctly */
        for (size_t i = 0; i < list.item_size; ++i) {
            __CPROVER_assert(((uint8_t *)list.data)[i] == out[i],
                             "front element matches");
        }
    } else {
        /* On failure the list must be empty */
        __CPROVER_assert(list.length == 0, "list length == 0 on failure");
    }

    /* The list must remain unchanged */
    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(list.length == old.length);
    assert(list.current_size == old.current_size);
    assert(list.data == old.data);

    assert(aws_array_list_is_valid(&list));

    free(out);
}
