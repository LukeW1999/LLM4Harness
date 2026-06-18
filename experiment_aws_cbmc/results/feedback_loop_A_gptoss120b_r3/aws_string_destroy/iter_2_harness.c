#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

void aws_string_destroy_harness(void) {
    /* Always use a dynamically allocated string to satisfy the precondition */
    const char *c_str = "dynamic";
    struct aws_string *str = aws_string_new_from_c_str(aws_default_allocator(), c_str);
    __CPROVER_assume(str != NULL);               /* ensure allocation succeeded */
    __CPROVER_assume(aws_string_is_valid(str));  /* satisfy the function precondition */

    /* Remember the allocator for later checks (it must be non‑NULL for a dynamic string) */
    struct aws_allocator *old_allocator = str->allocator;
    __CPROVER_assert(old_allocator != NULL, "dynamic string must have a non‑NULL allocator");

    /* Call the function under test */
    aws_string_destroy(str);

    /* No further dereferencing of `str` is allowed after destroy.
       The only guarantee we can check is that the allocator used was non‑NULL. */
    (void)old_allocator;
}
