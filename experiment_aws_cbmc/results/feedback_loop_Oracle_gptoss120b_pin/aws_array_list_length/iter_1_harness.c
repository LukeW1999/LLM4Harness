#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_ITEM_SIZE   64U
#define MAX_INIT_ITEMS  10U

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    bool zeroed = nondet_bool();

    if (zeroed) {
        /* Completely zeroed list – allowed by the precondition */
        memset(&list, 0, sizeof(list));
    } else {
        /* Build a dynamically allocated, valid list */
        struct aws_allocator *alloc = aws_default_allocator();

        size_t item_size = nondet_size_t();
        __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

        size_t init_items = nondet_size_t();
        __CPROVER_assume(init_items <= MAX_INIT_ITEMS);

        /* Initialise the list – this may fail, but we ignore the error for the harness */
        (void)aws_array_list_init_dynamic(&list, alloc, init_items, item_size);

        /* Choose a length that is within the current capacity */
        size_t capacity = (list.current_size == 0) ? 0 : list.current_size / list.item_size;
        size_t length = nondet_size_t();
        __CPROVER_assume(length <= capacity);
        list.length = length;
    }

    /* Snapshot the list structure and its data buffer (if any) */
    struct aws_array_list pre_list = list;
    uint8_t *pre_data = NULL;
    if (list.current_size > 0) {
        pre_data = malloc(list.current_size);
        __CPROVER_assume(pre_data != NULL);
        memcpy(pre_data, list.data, list.current_size);
    }

    /* Call the function under test */
    size_t ret_len = aws_array_list_length(&list);

    /* 1. Return value / error code correctness */
    assert(ret_len == list.length);

    /* 2. Output buffer length/capacity invariants */
    /* The length field must still be within capacity */
    if (list.current_size > 0) {
        assert(list.length <= list.current_size / list.item_size);
    } else {
        assert(list.length == 0);
    }

    /* 3. Memory not modified beyond the function's contract (frame condition) */
    /* The list structure (except for length which is unchanged) must be unchanged */
    assert(memcmp(&pre_list, &list, sizeof(struct aws_array_list)) == 0);

    /* The data buffer must be unchanged */
    if (pre_data != NULL) {
        assert(memcmp(pre_data, list.data, list.current_size) == 0);
        free(pre_data);
    }

    /* The list must still satisfy its validity predicate */
    assert(aws_array_list_is_valid(&list));

    return 0;
}
