#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    ensure_array_list_is_allocated(&list);
    list.alloc = aws_default_allocator();
    __CPROVER_assume(aws_array_list_is_valid(&list));
    
    __CPROVER_assume(list.item_size > 0);
    __CPROVER_assume(list.item_size <= 10);
    __CPROVER_assume(list.length <= 10);
    __CPROVER_assume(list.current_size <= 10);
    __CPROVER_assume(list.length <= list.current_size);
    __CPROVER_assume(list.current_size == 0 || list.data != NULL);
    __CPROVER_assume(list.data == NULL || __CPROVER_is_dynamic_object(list.data));

    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= 10);
    
    uint8_t val[10];

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        if (index < old.length) {
            assert(list.length == old.length);
        } else {
            assert(list.length == index + 1);
        }
    } else {
        assert(aws_array_list_is_valid(&list));
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.length == old.length);
    }
}
