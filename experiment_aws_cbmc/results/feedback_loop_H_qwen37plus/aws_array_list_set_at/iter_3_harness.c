#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    list.alloc = aws_default_allocator();
    ensure_array_list_is_allocated(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    __CPROVER_assume(list.length <= 10);
    __CPROVER_assume(list.current_size <= 10);

    struct aws_array_list old = list;

    size_t index = nondet_size_t();
    __CPROVER_assume(index <= 10);
    uint8_t val = nondet_uint8_t();

    int result = aws_array_list_set_at(&list, &val, index);

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
