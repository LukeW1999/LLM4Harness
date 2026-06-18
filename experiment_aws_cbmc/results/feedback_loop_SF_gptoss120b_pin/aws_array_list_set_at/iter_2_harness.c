#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/nondet.h"
#include "proof_helpers/utils.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_array_list_set_at_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_array_list list;
    ensure_array_list_is_valid(&list, alloc);

    size_t index = nondet_size_t();

    void *val = NULL;
    if (list.item_size > 0) {
        val = malloc(list.item_size);
        __CPROVER_assume(val != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(val, list.item_size));
    }

    size_t old_length = list.length;
    void *old_data = list.data;

    aws_array_list_set_at(&list, val, index);

    assert(list.length == old_length);
    assert(list.data == old_data);

    if (index < old_length && list.item_size > 0) {
        assert(!memcmp((unsigned char *)list.data + index * list.item_size, val, list.item_size));
    }

    if (val) {
        free(val);
    }
    aws_array_list_clean_up(&list);
}
