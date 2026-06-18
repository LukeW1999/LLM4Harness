#include <aws/common/common.h>
#include <aws/common/linked_list.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_linked_list_init_harness(void) {
    /* Allocate the list under test */
    struct aws_linked_list list;

    /* Nondeterministically initialize the whole structure */
    for (size_t i = 0; i < sizeof(list); ++i) {
        ((uint8_t *)&list)[i] = (uint8_t) nondet_uint8_t();
    }

    /* Save a copy of unrelated memory to check frame conditions */
    uint8_t untouched[32];
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        untouched[i] = (uint8_t) nondet_uint8_t();
    }
    uint8_t untouched_before[32];
    for (size_t i = 0; i < sizeof(untouched_before); ++i) {
        untouched_before[i] = untouched[i];
    }

    /* Call the function under verification */
    aws_linked_list_init(&list);

    /* Post‑conditions */
    assert(aws_linked_list_is_valid(&list));
    assert(aws_linked_list_empty(&list));

    /* Structural invariants imposed by the implementation */
    assert(list.head.next == &list.tail);
    assert(list.head.prev == NULL);
    assert(list.tail.prev == &list.head);
    assert(list.tail.next == NULL);

    /* Frame condition: memory outside the list must be unchanged */
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        assert(untouched[i] == untouched_before[i]);
    }

    return 0;
}
