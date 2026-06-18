#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_list_set_at_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();
    list.alloc = alloc;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    void *val;
    if (list.item_size == 0) {
        val = NULL;
    } else {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    size_t index = nondet_size_t();

    struct aws_array_list old = list;
    size_t old_length = list.length;

    int result = aws_array_list_set_at(&list, val, index);

    assert(list.alloc == old.alloc);
    assert(list.item_size == old.item_size);
    assert(aws_array_list_is_valid(&list));

    if (result == AWS_OP_SUCCESS) {
        if (index >= old_length) {
            assert(list.length == index + 1);
        } else {
            assert(list.length == old_length);
        }
        assert(list.data != NULL);
    } else {
        assert(list.length == old_length);
        assert(list.data == old.data);
    }
}
