#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static size_t nondet_size_t(void);

void aws_array_list_length_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);

    list.length = nondet_size_t();
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);

    if (list.current_size > 0) {
        size_t total_bytes = list.current_size * list.item_size;
        list.data = malloc(total_bytes);
        __CPROVER_assume(list.data != NULL);
        __CPROVER_assume(__CPROVER_is_writable(list.data, total_bytes));
    } else {
        list.data = NULL;
    }

    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old_list = list;

    uint8_t *old_data = NULL;
    if (list.data && list.current_size > 0) {
        size_t total_bytes = list.current_size * list.item_size;
        old_data = malloc(total_bytes);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, total_bytes);
    }

    size_t ret = aws_array_list_length(&list);

    assert(ret == list.length);
    assert(list.length == old_list.length);
    assert(list.current_size == old_list.current_size);
    assert(list.item_size == old_list.item_size);
    assert(list.alloc == old_list.alloc);

    if (list.data && list.current_size > 0) {
        size_t total_bytes = list.current_size * list.item_size;
        assert(old_data != NULL);
        assert(memcmp(old_data, list.data, total_bytes) == 0);
    }

    assert(aws_array_list_is_valid(&list));

    if (list.data) {
        free(list.data);
    }
    if (old_data) {
        free(old_data);
    }
}
