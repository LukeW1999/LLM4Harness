#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    void *val;
    size_t index;

    /* bound the array list */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* allocate readable memory for val of size list->item_size */
    val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    index = nondet_size_t();
    /* limit index to avoid unreasonable state space explosion */
    __CPROVER_assume(index < MAX_INITIAL_ITEM_ALLOCATION * 2);

    struct aws_array_list old_list = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((const uint8_t *)list.data, list.current_size, &old_byte);

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        /* element at index should be val */
        assert_bytes_match((const uint8_t *)list.data + index * list.item_size, val, list.item_size);

        /* length update: if index >= old length, length becomes index + 1 */
        if (index >= old_list.length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_list.length);
        }

        /* capacity must be at least enough for index+1 elements */
        assert(aws_array_list_capacity(&list) >= index + 1);

        /* rest of data unchanged (the helper checks that only element at index changed) */
        assert_array_list_equivalence(&list, &old_list, &old_byte);
    } else {
        /* failure: list completely unchanged */
        assert(list.data == old_list.data);
        assert(list.length == old_list.length);
        assert(list.current_size == old_list.current_size);
        assert(list.item_size == old_list.item_size);
        assert(list.alloc == old_list.alloc);
        assert_byte_from_buffer_matches((const uint8_t *)list.data, &old_byte);
    }

    assert(aws_array_list_is_valid(&list));
}
