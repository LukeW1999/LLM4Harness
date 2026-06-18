#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_back_harness(void) {
    struct aws_array_list list = {0};
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    /* Nondeterministic fields */
    list.alloc = alloc;
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.length = nondet_size_t();
    size_t extra = nondet_size_t();
    list.current_size = list.item_size * (list.length + extra);
    __CPROVER_assume(list.length * list.item_size <= list.current_size);

    /* Allocate backing storage if needed */
    if (list.current_size > 0) {
        list.data = malloc(list.current_size);
        __CPROVER_assume(list.data != NULL);
        uint8_t *buf = (uint8_t *)list.data;
        for (size_t i = 0; i < list.current_size; ++i) {
            buf[i] = nondet_uint8_t();
        }
    } else {
        list.data = NULL;
    }

    /* Ensure validity precondition */
    __CPROVER_assume(!list.length || list.data != NULL);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* Allocate writable output buffer */
    void *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    /* PRE-CALL SNAPSHOT */
    size_t old_length = list.length;
    size_t old_current_size = list.current_size;
    size_t old_item_size = list.item_size;
    void *old_data = list.data;
    uint8_t *old_data_copy = NULL;
    if (list.data != NULL) {
        old_data_copy = malloc(list.current_size);
        __CPROVER_assume(old_data_copy != NULL);
        memcpy(old_data_copy, list.data, list.current_size);
    }

    /* CALL */
    int result = aws_array_list_back(&list, val);

    /* POSTCONDITIONS */
    assert((result == 0) == (old_length > 0));

    assert(list.alloc == alloc);
    assert(list.item_size == old_item_size);
    assert(list.length == old_length);
    assert(list.current_size == old_current_size);
    assert(list.data == old_data);

    if (old_data != NULL) {
        assert(memcmp(list.data, old_data_copy, old_current_size) == 0);
    }

    if (result == 0) {
        assert(memcmp(val,
                      (uint8_t *)old_data + (old_length - 1) * old_item_size,
                      old_item_size) == 0);
    }
}
