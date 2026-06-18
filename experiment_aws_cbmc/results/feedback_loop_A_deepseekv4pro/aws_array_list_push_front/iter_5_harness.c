#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t item_size = list.item_size;
    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, item_size));

    struct aws_array_list old = list;
    size_t old_length = old.length;
    size_t old_current_size = old.current_size;

    uint8_t *old_data = NULL;
    size_t old_data_size = old_length * item_size;
    if (old_data_size > 0) {
        old_data = malloc(old_data_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_memcpy(old_data, old.data, old_data_size);
    }

    int result = aws_array_list_push_front(&list, val);

    assert(list.item_size == old.item_size);
    assert(list.alloc == old.alloc);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old_length + 1);
        assert(list.current_size >= old_current_size);
        assert_bytes_match(list.data, val, item_size);
        if (old_length > 0) {
            for (size_t i = 0; i < old_length; i++) {
                assert_bytes_match((uint8_t *)list.data + (i + 1) * item_size,
                                   old_data + i * item_size,
                                   item_size);
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == old_length);
        assert(list.current_size == old_current_size);
        if (old_data_size > 0) {
            assert_bytes_match(list.data, old_data, old_data_size);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    if (old_data != NULL) {
        free(old_data);
    }
}
