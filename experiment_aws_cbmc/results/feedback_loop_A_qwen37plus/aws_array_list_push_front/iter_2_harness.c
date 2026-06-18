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
    list.alloc = aws_default_allocator();
    list.item_size = nondet_size_t();
    __CPROVER_assume(list.item_size > 0);
    list.current_size = nondet_size_t();
    __CPROVER_assume(list.current_size <= (SIZE_MAX / list.item_size));
    list.length = nondet_size_t();
    __CPROVER_assume(list.length <= list.current_size);
    
    if (list.current_size > 0) {
        list.data = malloc(list.current_size * list.item_size);
        __CPROVER_assume(list.data != NULL);
    } else {
        list.data = NULL;
    }
    
    __CPROVER_assume(aws_array_list_is_valid(&list));

    uint8_t *val = malloc(list.item_size);
    __CPROVER_assume(val != NULL);

    struct aws_array_list old = list;

    int result = aws_array_list_push_front(&list, val);

    if (result == AWS_OP_SUCCESS) {
        assert(list.length == old.length + 1);
        assert_bytes_match((const uint8_t *)list.data, val, list.item_size);
    } else {
        assert(list.length == old.length);
        assert(list.data == old.data);
        assert(list.current_size == old.current_size);
    }

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);

    assert(aws_array_list_is_valid(&list));

    free(list.data);
    free(val);
}
