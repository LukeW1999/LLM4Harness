#include <aws/common/array_list.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 1024
#define MAX_ITEM_SIZE               256

extern size_t nondet_size_t(void);

void aws_array_list_init_static_harness(void) {
    struct aws_array_list list;

    /* nondet inputs */
    size_t item_count = nondet_size_t();
    size_t item_size  = nondet_size_t();

    /* ground‑truth preconditions */
    __CPROVER_assume(item_count > 0 && item_count <= MAX_INITIAL_ITEM_ALLOCATION);
    __CPROVER_assume(item_size > 0 && item_size <= MAX_ITEM_SIZE);

    size_t len;
    __CPROVER_assume(!aws_mul_size_checked(item_count, item_size, &len));

    /* raw array allocation */
    void *raw_array = malloc(len);
    __CPROVER_assume(raw_array != NULL);

    /* snapshot of raw array contents */
    unsigned char *raw_before = malloc(len);
    __CPROVER_assume(raw_before != NULL);
    memcpy(raw_before, raw_array, len);

    /* function under test */
    aws_array_list_init_static(&list, raw_array, item_count, item_size);

    /* post‑conditions */
    assert(aws_array_list_is_valid(&list));
    assert(list.length == 0);
    assert(list.item_size == item_size);
    assert(list.current_size == len);
    assert(list.data == raw_array);
    assert(list.alloc == NULL);
    assert(aws_array_list_capacity(&list) == item_count);

    /* raw array must remain unchanged */
    assert(memcmp(raw_before, raw_array, len) == 0);

    return 0;
}
