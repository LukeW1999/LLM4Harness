#include <aws/common/array_list.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_INITIAL_ITEM_ALLOCATION 10U
#define MAX_ITEM_SIZE               64U

void aws_array_list_clean_up_harness(void) {
    struct aws_array_list list;

    __CPROVER_assume(aws_array_list_is_bounded(&list,
                                               MAX_INITIAL_ITEM_ALLOCATION,
                                               MAX_ITEM_SIZE));
    __CPROVER_assume(aws_array_list_is_valid(&list));

    struct aws_array_list old = list;
    uint8_t *old_data = NULL;
    size_t old_data_size = 0;

    if (list.data != NULL) {
        old_data_size = list.current_size;
        old_data = malloc(old_data_size);
        __CPROVER_assume(old_data != NULL);
        memcpy(old_data, list.data, old_data_size);
    }

    aws_array_list_clean_up(&list);

    assert(list.alloc == NULL);
    assert(list.data == NULL);
    assert(list.current_size == 0U);
    assert(list.length == 0U);
    assert(list.item_size == 0U);

    if (old_data != NULL) {
        assert(memcmp(old_data, old_data, old_data_size) == 0);
        free(old_data);
    }

    assert(old.alloc != list.alloc);
}
