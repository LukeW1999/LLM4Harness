# Feature Definitions — Phase 2

Two groups corresponding to the two-barrier hypothesis.
All features documented with: definition, computation, examples.

---

## Group 1 — Knowledge-barrier features

### f_var_overlap [continuous, 0–1]
**Definition**: Fraction of identifier tokens in the assertion expression that also
appear in the function's implementation body text.
High = assertion variables are visible in code; Low = assertion uses variables not in code.

**Computation**: `extract_identifiers(expr_norm)` → intersect with tokens in `impl_text`.

**Examples (high ≈ derivable)**:
- `buf.len == 0` in `aws_byte_buf_init`: `len`, `buf` both in impl → 1.0
- `r == a + b` in `aws_add_size_checked`: `a`, `b`, `r` all in impl → 1.0

**Examples (low ≈ not derivable)**:
- `aws_byte_buf_is_valid(&_arg_)`: predicate name not in impl body → low

---

### f_field_overlap [continuous, 0–1, NA if no fields]
**Definition**: Fraction of struct field names (e.g. `.len`, `.capacity`) in the
assertion that appear in the function implementation.

**Computation**: regex `\w+\.(\w+)` → intersect with impl tokens.

---

### f_predicate_in_impl [binary, -1=NA]
**Definition**: 1 if any validity predicate (`*_is_valid`, `*_validate`) in the assertion
expression also appears anywhere in the function's implementation text.
-1 if expression has no validity predicate.

**Computation**: regex for predicate names → `in impl_text.lower()`.

**Examples (1 — predicate visible in impl)**:
- `aws_byte_buf_is_valid` in `aws_byte_buf_init`: impl has `AWS_POSTCONDITION(aws_byte_buf_is_valid(buf))` → 1
- `aws_linked_list_node_next_is_valid` in `aws_linked_list_next`: impl has `AWS_POSTCONDITION(...)` → 1

**Examples (0 — predicate not in impl)**:
- `aws_linked_list_node_prev_is_valid` in `aws_linked_list_push_back`: predicate
  only in NL header, not in push_back's own impl → 0

---

### f_predicate_in_postcond [binary, -1=NA]
**Definition**: 1 if predicate appears specifically inside an `AWS_POSTCONDITION()` /
`POSIX_POSTCONDITION()` call in the function implementation.
Stronger than `f_predicate_in_impl` — only fires on explicit postcondition annotation.

---

### f_predicate_in_nl [binary, -1=NA]
**Definition**: 1 if predicate appears in NL `Ensures:` / `@ensures` / `postcondition`
annotations in the function's header declaration.

**Examples (1)**:
- `aws_linked_list_node_prev_is_valid` in header "Ensures: aws_linked_list_node_prev_is_valid(node)"

---

### f_uses_cbmc_builtin [binary]
**Definition**: 1 if expression contains `__cprover_*` CBMC-specific builtins.
These are never visible in production code text.

---

## Group 2 — Strategy-barrier features

### f_is_frame_negative [binary]
**Definition**: 1 if assertion expresses "something did NOT change" — i.e., a frame
condition of the form `old.X == new.X` or `OLD_field == current_field`.

**Computation**: regex for `old_\w+\s*==`, `==\s*old_\w+`, `OLD_\w+` patterns.

**Why it matters**: Frame conditions are CODE-derivable (you can read the impl to see
what doesn't change) but LLMs systematically skip them — this is the strategy barrier.
Prediction: high f_is_frame_negative → low recall under A/B/C/D, recoverable under E/F.

**Examples (1)**:
- `old_to.head == list.head` (head didn't change)
- `OLD_alloc == buf.allocator`

**Examples (0)**:
- `buf.len == 0` (postcondition about what changed)
- `aws_byte_buf_is_valid(&_arg_)` (validity check)

---

### f_position_post_call [binary, -1=NA]
**Definition**: 1 if this assertion appears textually AFTER the call to the function
under test in the harness. Preconditions appear before; postconditions appear after.

---

### f_negation_depth [int, 0+]
**Definition**: Count of `!` negation operators in the expression.
Higher negation depth → logically more complex condition.

---

### f_arity [int, 0+]
**Definition**: Number of arguments to the top-level function call in the expression.
0 if expression is not a function call. Higher arity = more complex predicate.
