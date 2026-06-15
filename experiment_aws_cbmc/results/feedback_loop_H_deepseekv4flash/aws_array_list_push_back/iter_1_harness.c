#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

void harness() {
    struct aws_array_list list;
    struct aws_array_list old;
    struct store_byte_from_buffer data_storage;

    /* Non-deterministic list bounded to avoid blow-up */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Ensure dynamic (alloc != NULL) for simplicity */
    __CPROVER_assume(list.alloc != NULL);

    /* Non-deterministic val readable of size item_size */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Non-deterministic index */
    size_t index;

    /* Save old state */
    old = list;
    save_byte_from_array((uint8_t *)old.data, old.current_size, &data_storage);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        /* Element at index must equal val */
        size_t offset = index * list.item_size;
        __CPROVER_assume(offset < list.current_size); /* capacity ensures this */
        assert_bytes_match((uint8_t *)list.data + offset, (uint8_t *)val, list.item_size);

        /* Length adjusted if index >= old length */
        if (index >= old.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old.length);
        }

        /* Unchanged fields */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert(list.current_size >= old.current_size);
    } else {
        /* On failure, list is unchanged */
        assert(list.current_size == old.current_size);
        assert(list.length == old.length);
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
        assert(list.data == old.data);
        assert_byte_from_buffer_matches((uint8_t *)list.data, &data_storage);
    }

    /* Validity invariant always holds */
    assert(aws_array_list_is_valid(&list));
}
