#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "aws/common/array_list.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_array_list_back_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    size_t val_capacity = list.item_size;
    uint8_t *val = malloc(val_capacity);
    __CPROVER_assume(val != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, val_capacity));

    struct aws_array_list old = list;

    uint8_t *saved_last_element = NULL;
    size_t last_element_offset;
    if (list.length > 0) {
        last_element_offset = list.item_size * (list.length - 1);
        saved_last_element = malloc(list.item_size);
        __CPROVER_assume(saved_last_element != NULL);
        memcpy(saved_last_element, (uint8_t *)list.data + last_element_offset, list.item_size);
    }

    int result = aws_array_list_back(&list, val);

    assert(aws_array_list_is_valid(&list));
    assert(list.alloc == old.alloc);
    assert(list.current_size == old.current_size);
    assert(list.length == old.length);
    assert(list.item_size == old.item_size);
    assert(list.data == old.data);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);
        for (size_t i = 0; i < list.item_size; i++) {
            assert(val[i] == saved_last_element[i]);
        }
        free(saved_last_element);
    } else {
        assert(list.length == 0);
    }

    free(val);
}
