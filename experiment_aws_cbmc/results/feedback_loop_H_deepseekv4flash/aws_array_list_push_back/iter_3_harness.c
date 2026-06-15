#include <aws/common/array_list.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10
#define MAX_ITEM_SIZE 4

void harness() {
    struct aws_array_list list;
    struct aws_array_list old;
    struct store_byte_from_buffer data_storage;

    /* Non-deterministic list, but ensure it is valid and has allocated data */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.alloc != NULL);
    __CPROVER_assume(list.item_size > 0);

    /* Non-deterministic val readable of size item_size */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));

    /* Save old state */
    old = list;
    save_byte_from_array((uint8_t *)old.data, old.current_size, &data_storage);

    /* Call push_back */
    int result = aws_array_list_push_back(&list, val);

    if (result == AWS_OP_SUCCESS) {
        /* Length increased by 1 */
        assert(list.length == old.length + 1);
        /* New element at the end matches val */
        size_t offset = (list.length - 1) * list.item_size;
        assert_bytes_match((uint8_t *)list.data + offset, (uint8_t *)val, list.item_size);
        /* Data before the new element is unchanged (content, not pointer) */
        assert_bytes_match((uint8_t *)list.data, (uint8_t *)old.data, old.current_size);
        /* Other fields unchanged (except current_size and data may change if realloc) */
        assert(list.item_size == old.item_size);
        assert(list.alloc == old.alloc);
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
