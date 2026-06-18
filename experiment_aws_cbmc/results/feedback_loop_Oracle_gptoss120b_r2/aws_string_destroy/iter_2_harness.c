#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_string_destroy_harness(void) {
    /* Setup: nondeterministic string pointer with structural validity. */
    struct aws_string *str;
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    /* Save allocator pointer for later frame‑condition check. */
    struct aws_allocator *saved_allocator = NULL;
    if (str != NULL) {
        saved_allocator = str->allocator;
    }

    /* Frame condition: allocate a dummy buffer and snapshot its contents. */
    uint8_t dummy[16];
    for (size_t i = 0; i < sizeof(dummy); ++i) {
        dummy[i] = __CPROVER_nondet_uchar();
    }
    uint8_t dummy_snapshot[16];
    memcpy(dummy_snapshot, dummy, sizeof(dummy));

    /* Call the function under verification. */
    aws_string_destroy(str);

    /* Post‑condition checks: dummy buffer unchanged. */
    for (size_t i = 0; i < sizeof(dummy); ++i) {
        assert(dummy[i] == dummy_snapshot[i]);
    }

    /* The allocator pointer inside the string (if any) must not be altered. */
    if (str != NULL) {
        assert(str->allocator == saved_allocator);
    }
}
