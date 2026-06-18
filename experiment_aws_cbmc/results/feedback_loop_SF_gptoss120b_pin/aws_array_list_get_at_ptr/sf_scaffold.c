#include <aws/common/array_list.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

extern size_t nondet_size_t(void);
extern void *nondet_voidp(void);

void aws_array_list_get_at_ptr_harness(void) {
    struct aws_array_list list;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic initialization of list fields */
    list.alloc = alloc;
    list.item_size = nondet_size_t();
    list.length = nondet_size_t();
    list.current_size = nondet_size_t();
    list.data = malloc(list.current_size);
    __CPROVER_assume(list.current_size == 0 || list.data != NULL);

    /* ensure the list satisfies its validity predicate */
    __CPROVER_assume(aws_array_list_is_valid(&list));

    /* nondeterministic index */
    size_t index = nondet_size_t();

    /* output pointer */
    void *val = NULL;

    /* pre‑call snapshot */
    size_t old_length = list.length;
    void *old_data = list.data;

    /* call under verification */
    int result = aws_array_list_get_at_ptr(&list, &val, index);

    /* ASSERT_POSTCONDITIONS_HERE */
}
