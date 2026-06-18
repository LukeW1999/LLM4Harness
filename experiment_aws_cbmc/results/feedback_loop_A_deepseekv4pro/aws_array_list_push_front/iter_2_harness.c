#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_INITIAL_ITEM_ALLOCATION
#define MAX_INITIAL_ITEM_ALLOCATION 256
#endif

#ifndef MAX_ITEM_SIZE
#define MAX_ITEM_SIZE 256
#endif

void aws_array_list_push_front_harness() {
    struct aws_array_list list;

    size_t item_size;
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);
    size_t length;
    __CPROVER_assume(length <= MAX_INITIAL_ITEM_ALLOCATION);
    size_t current_size;
    __CPROVER_assume(length <= current_size && current_size <= MAX_INITIAL_ITEM_ALLOCATION);

    list.data = malloc(current_size * item_size);
    __CPROVER_assume(list.data != NULL);
    list.length = length;
    list.current_size = current_size;
    list.item_size = item_size;
    list.alloc = aws_default_allocator();

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    size_t old_data_size = old.length * old.item_size;
    uint8_t *old_data = NULL;
    if (old_data_size > 0) {
        old_data = malloc(old_data_size);
        __CPROVER_assume(old_data != NULL);
        __CPROVER_memcpy(old_data, old.data, old_data_size);
    }

    uint8_t *val = malloc(item_size);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(__CPROVER_r_ok(val, item_size));

    int result = aws_array_list_push_front(&list, val);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert_bytes_match(list.data, val, item_size);
        if (old.length > 0) {
            for (size_t i = 0; i < old.length; i++) {
                assert_bytes_match((uint8_t *)list.data + (i + 1) * item_size,
                                   old_data + i * old.item_size,
                                   item_size);
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == old.length);
        assert(list.current_size == old.current_size);
        assert(list.data == old.data);
        if (old_data_size > 0) {
            assert_bytes_match(list.data, old_data, old_data_size);
        }
    }

    assert(aws_array_list_is_valid(&list));

    free(val);
    if (old_data) free(old_data);
}
