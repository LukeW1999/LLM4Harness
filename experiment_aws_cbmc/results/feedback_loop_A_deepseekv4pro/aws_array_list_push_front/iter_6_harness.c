#include <aws/common/common.h>
#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_array_list list;

    /* set up a bounded, valid array list with default allocator */
    list.alloc = alloc;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* snapshot the old state */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t item_size = list.item_size;

    /* allocate and populate the new element */
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    for (size_t i = 0; i < item_size; i++) {
        val[i] = nondet_uint8();
    }

    /* keep a copy of the old contents for later comparison */
    uint8_t *old_data = NULL;
    if (old_length > 0) {
        old_data = malloc(old_length * item_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_memcpy(old_data, list.data, old_length * item_size);
    }

    /* call the function under verification */
    int result = aws_array_list_push_front(&list, val);

    /* post-condition checks */
    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
        assert(list.current_size >= old_current_size);
        /* the first element must match the pushed value */
        assert_bytes_match(list.data, val, item_size);
        /* subsequent elements must be the old contents in order */
        if (old_length > 0) {
            for (size_t i = 0; i < old_length; i++) {
                assert_bytes_match((uint8_t *)list.data + (i + 1) * item_size,
                                   old_data + i * item_size,
                                   item_size);
            }
        }
    } else {
        /* With aws_default_allocator() this path is unreachable,
         * but we keep it for completeness of the specification.
         */
        assert(result == AWS_OP_ERR);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        if (old_length > 0) {
            assert_bytes_match(list.data, old_data, old_length * item_size);
        }
    }

    /* the list must remain valid after the operation */
    assert(aws_array_list_is_valid(&list));

    /* clean up */
    free(val);
    if (old_data != NULL) {
        free(old_data);
    }
    /* list.data was allocated by ensure_array_list_has_allocated_data_member */
    free(list.data);
}
