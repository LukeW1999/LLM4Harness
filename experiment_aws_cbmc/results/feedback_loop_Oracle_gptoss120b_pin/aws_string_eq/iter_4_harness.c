#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

void aws_string_eq_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministically create two aws_string objects */
    struct aws_string *str_a = make_aws_string_nondet();
    struct aws_string *str_b = make_aws_string_nondet();

    /* structural validity assumptions */
    __CPROVER_assume(IMPLIES(str_a != NULL, aws_string_is_valid(str_a)));
    __CPROVER_assume(IMPLIES(str_b != NULL, aws_string_is_valid(str_b)));

    /* bound lengths to keep verification tractable */
    __CPROVER_assume(str_a == NULL || str_a->len <= 16);
    __CPROVER_assume(str_b == NULL || str_b->len <= 16);

    /* snapshot state of a */
    size_t len_a_snapshot = 0;
    uint8_t bytes_a_snapshot[16];
    bool have_a_snapshot = false;
    if (str_a != NULL) {
        len_a_snapshot = str_a->len;
        if (len_a_snapshot > 0) {
            memcpy(bytes_a_snapshot, str_a->bytes, len_a_snapshot);
            have_a_snapshot = true;
        }
    }

    /* snapshot state of b */
    size_t len_b_snapshot = 0;
    uint8_t bytes_b_snapshot[16];
    bool have_b_snapshot = false;
    if (str_b != NULL) {
        len_b_snapshot = str_b->len;
        if (len_b_snapshot > 0) {
            memcpy(bytes_b_snapshot, str_b->bytes, len_b_snapshot);
            have_b_snapshot = true;
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq(str_a, str_b);

    /* -------------------------------------------------------------------- */
    /* Postcondition 1: return value correctness */
    __CPROVER_assert(
        (result == true) == (
            (str_a == str_b) ||
            (str_a != NULL && str_b != NULL &&
             str_a->len == str_b->len &&
             (str_a->len == 0 ||
              memcmp(str_a->bytes, str_b->bytes, str_a->len) == 0))
        ),
        "aws_string_eq return value matches equality of strings");

    /* -------------------------------------------------------------------- */
    /* Postcondition 2: length invariants (no change to lengths) */
    if (str_a != NULL) {
        __CPROVER_assert(str_a->len == len_a_snapshot,
                         "aws_string_eq does not modify a->len");
    }
    if (str_b != NULL) {
        __CPROVER_assert(str_b->len == len_b_snapshot,
                         "aws_string_eq does not modify b->len");
    }

    /* -------------------------------------------------------------------- */
    /* Postcondition 3: frame condition (bytes unchanged) */
    if (str_a != NULL && have_a_snapshot) {
        __CPROVER_assert(
            memcmp(str_a->bytes, bytes_a_snapshot, len_a_snapshot) == 0,
            "aws_string_eq does not modify a->bytes");
    }
    if (str_b != NULL && have_b_snapshot) {
        __CPROVER_assert(
            memcmp(str_b->bytes, bytes_b_snapshot, len_b_snapshot) == 0,
            "aws_string_eq does not modify b->bytes");
    }
}
