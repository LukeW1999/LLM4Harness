#include <aws/common/array_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_array_list_set_at_harness() {
    struct aws_array_list list;
    AWS_ZERO_STRUCT(list);

    struct aws_allocator *allocator = aws_default_allocator();
    list.alloc = allocator;

    /* Choose a non‑zero item size for the list */
    list.item_size = (size_t)nondet_uint();
    __CPROVER_assume(list.item_size > 0);

    /* Allocate the underlying data buffer for the list */
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;

    /* Choose an index that may be within or just beyond the current length */
    size_t index = (size_t)nondet_uint();
    __CPROVER_assume(index <= list.length);

    /* Allocate a value of the appropriate size */
    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
    }

    int result = aws_array_list_set_at(&list, val, index);

    if (result == AWS_OP_SUCCESS) {
        if (index >= old.length) {
            assert(list.length == old.length + 1);
        } else {
            assert(list.length == old.length);
        }
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(aws_array_list_is_valid(&list));
    } else {
        assert(list.length == old.length);
        assert(list.alloc == old.alloc);
        assert(list.item_size == old.item_size);
        assert(list.data == old.data);
        assert(aws_array_list_is_valid(&list));
    }

    free(val);
}
