#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 256
#endif

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 256
#endif

void aws_array_list_push_back_harness() {
    struct aws_array_list list;

    /* ensure the list is bounded and has valid backing data */
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));

    /* allocate and initialize the list data member */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t old_length = aws_array_list_length(&list);
    size_t old_current_size = list.current_size;
    size_t item_size = list.item_size;

    /* create a non-deterministic item to push */
    uint8_t item[MAX_ITEM_SIZE];
    __CPROVER_assume(item_size <= MAX_ITEM_SIZE);

    /* capture the state before push */
    struct aws_array_list old = list;
    uint8_t old_data[MAX_INITIAL_ITEM_ALLOCATION * MAX_ITEM_SIZE];
    __CPROVER_assume(list.current_size <= sizeof(old_data));
    memcpy(old_data, list.data, list.current_size);

    int result = aws_array_list_push_back(&list, item);

    /* structural invariants must hold */
    assert(aws_array_list_is_valid(&list));
    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        /* length increased by 1 */
        assert(list.length == old_length + 1);
        /* current_size may have increased if reallocation happened, but at least enough for new item */
        assert(list.current_size >= (old_length + 1) * item_size);
        /* the new item is at the end */
        size_t i;
        for (i = 0; i < item_size; i++) {
            assert(((uint8_t *)list.data)[old_length * item_size + i] == item[i]);
        }
        /* the previous items are unchanged */
        for (i = 0; i < old_length * item_size; i++) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    } else {
        /* push failed, list should be unchanged */
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        size_t i;
        for (i = 0; i < old_current_size; i++) {
            assert(((uint8_t *)list.data)[i] == old_data[i]);
        }
    }
}
