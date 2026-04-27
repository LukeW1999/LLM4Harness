#!/usr/bin/env python3
"""
annotate_missed_properties.py
Annotates all missed GT properties from condA_claude with the 2D taxonomy.

2D Taxonomy:
  info_source:  NL | CODE | DOMAIN
  reasoning:    DIRECT | CONDITIONAL | EXHAUSTIVE | STRUCTURAL

Category labels (semantic):
  FRAME_COND     - frame condition: unchanged struct field assertion
  VALIDITY_PRED  - AWS/CBMC validity predicate call (aws_X_is_valid, aws_is_zeroed, etc.)
  CBMC_BUILTIN   - CBMC-specific intrinsic (__cprover_overflow_mult, aws_mem_is_writable)
  STRUCT_PTR     - structural pointer relationship (linked list next/prev pointers)
  LEN_CHANGE     - length/size change postcondition (documented in NL or code)
  LEN_INVARIANT  - length/size invariant or equality check (not a frame cond)
  OVERFLOW_ARITH - arithmetic overflow/saturation correctness
  RETVAL_SPEC    - return value specification
  ZERO_CHECK     - memory zeroing postcondition
  TYPE_VARIANT   - 32-bit vs 64-bit type-specific testing
"""

import json, csv
from pathlib import Path

DATASET = Path('/home/weiqi/SpecVerify/experiment_aws_cbmc/dataset')
RESULTS = Path('/home/weiqi/SpecVerify/experiment_aws_cbmc/evaluation')
OUT_DIR = Path('/home/weiqi/SpecVerify/experiment_aws_cbmc/annotation')
OUT_DIR.mkdir(exist_ok=True)

# ─────────────────────────────────────────────────────────────────────────────
# Full manual annotations: (property_text_fragment, func) → (info, reasoning, category, note)
# property_text_fragment is a lowercase substring that uniquely identifies the property
# ─────────────────────────────────────────────────────────────────────────────

# Format: list of (func, property_substring, info_source, reasoning, category, note)
# If func is None, the rule applies to any function with that property substring.
ANNOTATIONS = [
    # ── DOMAIN / STRUCTURAL: AWS validity predicates ──────────────────────────
    # These require knowing the AWS CBMC proof library predicates exist
    (None, "aws_byte_buf_is_valid",          "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "Calls AWS proof-helper validity predicate; LLM unaware this idiom is required in CBMC harnesses"),
    (None, "aws_byte_cursor_is_valid",       "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "Same: AWS validity predicate for byte_cursor"),
    (None, "aws_linked_list_is_valid",       "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS validity predicate for linked_list"),
    (None, "aws_linked_list_node_prev_is_valid", "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "Node-level validity: checks prev pointer not null and not circular"),
    (None, "aws_linked_list_node_next_is_valid", "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "Node-level validity: checks next pointer not null and not circular"),
    (None, "aws_ring_buffer_is_valid",       "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS validity predicate for ring_buffer"),
    (None, "aws_string_is_valid",            "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS validity predicate for aws_string"),
    (None, "aws_c_string_is_valid",          "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS validity predicate for C strings"),
    (None, "aws_byte_buf_has_allocator",     "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS proof-helper: checks buf has a non-null allocator"),
    (None, "aws_is_zeroed",                  "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS proof-helper: checks all bytes of struct are zero; not documented in NL"),
    (None, "aws_mem_is_writable",            "DOMAIN", "STRUCTURAL", "CBMC_BUILTIN",
     "CBMC memory-model predicate; requires CBMC-specific knowledge"),
    (None, "aws_bytes_eq",                   "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "AWS proof-helper byte comparison; not a stdlib call"),
    (None, "aws_ring_buffer_buf_belongs_to_pool", "DOMAIN", "STRUCTURAL", "VALIDITY_PRED",
     "Domain-specific pool membership check; requires knowing ring_buffer proof idioms"),

    # ── DOMAIN / STRUCTURAL: CBMC builtins ────────────────────────────────────
    (None, "__cprover_overflow_mult",        "DOMAIN", "STRUCTURAL", "CBMC_BUILTIN",
     "CBMC intrinsic for overflow detection; not derivable from C code or NL"),

    # ── CODE / EXHAUSTIVE: Frame conditions (unchanged struct fields) ──────────
    # Pattern: old_X.field == X.field  or  X.field == old.field
    # These require enumerating ALL fields that the function does NOT modify
    ("aws_byte_buf_append",    "old_to.len == to.len",        "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Failure-path frame cond: buf.len unchanged on error; LLM misses failure-path exhaustion"),
    ("aws_byte_buf_append",    "old_to.allocator == to.allocator", "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Allocator not modified; requires knowing which fields are immutable"),
    ("aws_byte_buf_append",    "old_to.capacity == to.capacity",   "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Capacity not modified by append"),
    ("aws_byte_buf_append",    "old_from.len == from.len",          "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Source cursor not consumed/modified; frame cond for input arg"),

    ("aws_byte_buf_reset",     "buf.allocator == old.allocator",    "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Reset only zeroes len; allocator/buffer/capacity stay; LLM misses these"),
    ("aws_byte_buf_reset",     "buf.buffer == old.buffer",          "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Same: buffer pointer unchanged by reset"),
    ("aws_byte_buf_reset",     "buf.capacity == old.capacity",      "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Same: capacity unchanged by reset"),

    ("aws_array_list_clear",   "list.alloc == old.alloc",           "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Clear only sets length to 0; allocator/size/data stay"),
    ("aws_array_list_clear",   "list.current_size == old.current_size", "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: current_size (allocated memory) unchanged"),
    ("aws_array_list_clear",   "list.item_size == old.item_size",   "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: item_size unchanged"),
    ("aws_array_list_clear",   "list.data == old.data",             "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: data pointer unchanged"),

    ("aws_array_list_pop_back","list.alloc == old.alloc",           "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Pop only decrements length; alloc/current_size/item_size stay"),
    ("aws_array_list_pop_back","list.current_size == old.current_size", "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: allocated storage unchanged"),
    ("aws_array_list_pop_back","list.item_size == old.item_size",   "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: item_size unchanged"),

    ("aws_array_list_erase",   "list.item_size == old.item_size",   "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Erase shifts items but keeps item_size/alloc/current_size"),
    ("aws_array_list_erase",   "list.alloc == old.alloc",           "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: allocator unchanged"),
    ("aws_array_list_erase",   "list.current_size == old.current_size", "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: allocated bytes unchanged"),

    ("aws_array_list_pop_front","list.alloc == old.alloc",          "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Pop_front frame: alloc unchanged"),
    ("aws_array_list_pop_front","list.current_size == old.current_size","CODE","EXHAUSTIVE","FRAME_COND",
     "Frame: current_size unchanged"),
    ("aws_array_list_pop_front","list.item_size == old.item_size",  "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: item_size unchanged"),

    ("aws_array_list_pop_front_n","list.alloc == old.alloc",        "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Pop_front_n frame: alloc unchanged"),
    ("aws_array_list_pop_front_n","list.current_size == old.current_size","CODE","EXHAUSTIVE","FRAME_COND",
     "Frame: current_size unchanged"),
    ("aws_array_list_pop_front_n","list.item_size == old.item_size","CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: item_size unchanged"),

    ("aws_array_list_swap",    "list.alloc == old.alloc",           "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Swap of two items; alloc/current_size unchanged"),
    ("aws_array_list_swap",    "list.current_size == old.current_size","CODE","EXHAUSTIVE","FRAME_COND",
     "Frame: current_size unchanged after swap"),

    ("aws_byte_buf_advance",   "buf.capacity == old.capacity",      "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Advance updates len; capacity/allocator stay"),
    ("aws_byte_buf_advance",   "buf.allocator == old.allocator",    "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: allocator unchanged"),

    ("aws_ring_buffer_buf_belongs_to_pool","ring_buf_old == ring_buf","CODE","EXHAUSTIVE","FRAME_COND",
     "Membership check: ring_buf not modified by query"),
    ("aws_ring_buffer_buf_belongs_to_pool","old_buf == buf",        "CODE", "EXHAUSTIVE", "FRAME_COND",
     "Frame: buf not modified by membership check"),

    ("aws_ring_buffer_release","ring_buf.allocator == ring_buf_old.allocator","CODE","EXHAUSTIVE","FRAME_COND",
     "Release updates tail pointer; allocator/allocation/allocation_end stay"),
    ("aws_ring_buffer_release","ring_buf.allocation == ring_buf_old.allocation","CODE","EXHAUSTIVE","FRAME_COND",
     "Frame: allocation pointer unchanged"),
    ("aws_ring_buffer_release","ring_buf.allocation_end == ring_buf_old.allocation_end","CODE","EXHAUSTIVE","FRAME_COND",
     "Frame: allocation_end unchanged"),

    # ── CODE / STRUCTURAL: Linked list pointer relationships ──────────────────
    # These require tracing through the linked list manipulation in the implementation
    ("aws_linked_list_push_back", "list.tail.prev == &to_add",     "CODE", "STRUCTURAL", "STRUCT_PTR",
     "After push_back, tail.prev must point to new node; requires reading impl"),
    ("aws_linked_list_push_back", "to_add.prev == old_last",       "CODE", "STRUCTURAL", "STRUCT_PTR",
     "New node's prev must point to previous last; requires impl trace"),

    ("aws_linked_list_push_front","list.head.next == &to_add",     "CODE", "STRUCTURAL", "STRUCT_PTR",
     "After push_front, head.next must point to new node"),
    ("aws_linked_list_push_front","to_add.next == old_first",      "CODE", "STRUCTURAL", "STRUCT_PTR",
     "New node's next must point to previous first"),

    ("aws_linked_list_pop_back",  "ret.next == null && ret.prev == null","CODE","STRUCTURAL","STRUCT_PTR",
     "Detached node: pointers zeroed after removal"),
    ("aws_linked_list_pop_back",  "list.tail.prev == old_prev_last","CODE","STRUCTURAL","STRUCT_PTR",
     "New tail.prev after removal"),

    ("aws_linked_list_pop_front", "ret.next == null && ret.prev == null","CODE","STRUCTURAL","STRUCT_PTR",
     "Detached node: pointers zeroed after removal"),
    ("aws_linked_list_pop_front", "list.head.next == old_next_first","CODE","STRUCTURAL","STRUCT_PTR",
     "New head.next after removal"),

    ("aws_linked_list_back",   "back == old_last",                  "CODE", "STRUCTURAL", "STRUCT_PTR",
     "Return value is previous last node"),
    ("aws_linked_list_begin",  "rval == list.head.next",            "CODE", "DIRECT",     "STRUCT_PTR",
     "begin() returns sentinel head's next; straightforward but requires knowing sentinel pattern"),
    ("aws_linked_list_end",    "rval == &list.tail",                "CODE", "DIRECT",     "STRUCT_PTR",
     "end() returns address of sentinel tail"),
    ("aws_linked_list_rbegin", "rval == list.tail.prev",            "CODE", "DIRECT",     "STRUCT_PTR",
     "rbegin() returns tail.prev"),
    ("aws_linked_list_rend",   "rval == &list.head",                "CODE", "DIRECT",     "STRUCT_PTR",
     "rend() returns address of sentinel head"),

    ("aws_linked_list_front",  "front == old_first",                "CODE", "STRUCTURAL", "STRUCT_PTR",
     "Return value is previous first node"),

    ("aws_linked_list_insert_after",  "after.next == &to_add",     "CODE", "STRUCTURAL", "STRUCT_PTR",
     "After insertion, after.next must point to new node"),
    ("aws_linked_list_insert_before", "before_prev.next == &to_add","CODE","STRUCTURAL", "STRUCT_PTR",
     "Before insertion: before_prev.next updated to new node"),

    ("aws_linked_list_next",   "rval == &after",                    "CODE", "DIRECT",     "STRUCT_PTR",
     "next() returns the node after; requires knowing struct layout"),
    ("aws_linked_list_prev",   "rval == &before",                   "CODE", "DIRECT",     "STRUCT_PTR",
     "prev() returns the node before"),

    ("aws_linked_list_remove", "prev.next == &next",                "CODE", "STRUCTURAL", "STRUCT_PTR",
     "After remove, the previous node's next skips the removed node"),

    ("aws_linked_list_swap_contents","old_b.tail.prev == old_b_head","CODE","STRUCTURAL","STRUCT_PTR",
     "Swap: sentinel linkage must be re-established for b"),
    ("aws_linked_list_swap_contents","a.head.next == old_b.head.next","CODE","STRUCTURAL","STRUCT_PTR",
     "Swap: a's head.next gets b's original head.next"),
    ("aws_linked_list_swap_contents","a.tail.prev == old_b.tail.prev","CODE","STRUCTURAL","STRUCT_PTR",
     "Swap: a's tail.prev gets b's original tail.prev"),
    ("aws_linked_list_swap_contents","old_a.tail.prev == old_a_head","CODE","STRUCTURAL","STRUCT_PTR",
     "Swap: sentinel linkage for a side"),
    ("aws_linked_list_swap_contents","b.head.next == old_a.head.next","CODE","STRUCTURAL","STRUCT_PTR",
     "Swap: b's head.next gets a's original head.next"),

    ("aws_ring_buffer_release","new_tail == old_buf.buffer + old_buf.capacity","CODE","STRUCTURAL","STRUCT_PTR",
     "Release advances tail pointer by buffer capacity; arithmetic on pointers"),

    ("aws_byte_cursor_from_string","cursor.ptr == str.bytes",       "CODE", "DIRECT",     "STRUCT_PTR",
     "Cursor ptr set to string's bytes field; requires knowing aws_string layout"),

    # ── CODE / CONDITIONAL: Type variants (32-bit vs 64-bit) ─────────────────
    ("aws_add_size_checked",  "uint64_max",   "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "GT tests both u64 and u32 paths separately; LLM uses size_t only, missing the dispatch"),
    ("aws_add_size_checked",  "uint32_max",   "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Same: u32 overflow condition missed"),
    ("aws_add_size_saturating","uint64_max",  "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Saturation to UINT64_MAX on overflow; LLM uses SIZE_MAX instead of split testing"),
    ("aws_add_size_saturating","uint32_max",  "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Same: u32 saturation case"),
    ("aws_add_size_saturating","r == a + b",  "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Success-path result for both u32 and u64 separately tested in GT"),
    ("aws_mul_size_saturating","uint64_max",  "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Multiplication saturation: u64 overflow → UINT64_MAX"),
    ("aws_mul_size_saturating","uint32_max",  "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Same: u32"),
    ("aws_mul_size_saturating","r == a * b",  "CODE", "CONDITIONAL", "TYPE_VARIANT",
     "Success-path multiplication result for u32/u64"),

    # ── CODE / CONDITIONAL: Arithmetic overflow / bounds ─────────────────────
    ("aws_is_power_of_two",   "rval == (popcount == 1)",            "CODE", "DIRECT",     "RETVAL_SPEC",
     "Return value defined in terms of popcount; requires understanding the algorithm"),
    ("aws_round_up_to_power_of_two","popcount == 1",                "CODE", "DIRECT",     "OVERFLOW_ARITH",
     "Result must be a power of two (popcount 1)"),
    ("aws_round_up_to_power_of_two","test_val <= result",           "CODE", "DIRECT",     "OVERFLOW_ARITH",
     "Result >= input: basic rounding property"),
    ("aws_round_up_to_power_of_two","test_val >= result >> 1",      "CODE", "DIRECT",     "OVERFLOW_ARITH",
     "Minimality: result is the smallest power of two >= input"),
    ("aws_round_up_to_power_of_two","test_val > ((size_max >> 1) + 1)","CODE","CONDITIONAL","OVERFLOW_ARITH",
     "Overflow edge case: input too large to round up; conditional path in GT"),

    ("aws_nospec_mask",       "(index >= bound) || (bound > (size_max / 2)) || (index > (size_max / 2))",
     "CODE", "CONDITIONAL", "OVERFLOW_ARITH",
     "Speculative execution mask: nospec returns 0 when this condition holds; algorithm-specific"),
    ("aws_nospec_mask",       "rval == uintptr_max",                "CODE", "CONDITIONAL", "OVERFLOW_ARITH",
     "Mask returns UINTPTR_MAX (all bits set) on non-overflow path"),
    ("aws_nospec_mask",       "!((index >= bound)",                 "CODE", "CONDITIONAL", "OVERFLOW_ARITH",
     "Negation of overflow condition for non-zero return path"),

    ("aws_byte_buf_cat",      "old_dest.capacity - old_dest.len",   "CODE", "CONDITIONAL", "OVERFLOW_ARITH",
     "GT asserts the capacity check condition explicitly; LLM skips this path condition"),

    # ── NL / DIRECT: Documented length/size postconditions ───────────────────
    ("aws_byte_buf_append",   "to.len == old_to.len + from.len",    "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented in Doxygen: on success, to.len increases by from.len"),
    ("aws_array_list_pop_back","list.length == old.length - 1",     "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: pop_back decrements length by 1"),
    ("aws_array_list_push_back","list.length == old.length + 1",    "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: push_back increments length by 1"),
    ("aws_array_list_erase",  "list.length == old.length - 1",      "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: erase decrements length"),
    ("aws_array_list_pop_front","list.length == old.length - 1",    "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: pop_front decrements length"),
    ("aws_array_list_pop_front_n","list.length == old.length - n",  "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: pop_front_n decrements length by n"),
    ("aws_array_list_push_front","list.length == old.length + 1",   "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: push_front increments length"),
    ("aws_byte_buf_advance",  "buf.len == old.len + len",            "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: advance increases len"),

    ("aws_byte_buf_from_array","buf.len == length",                  "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: buf.len set to length parameter"),
    ("aws_byte_buf_from_array","buf.capacity == length",             "CODE", "DIRECT",     "LEN_CHANGE",
     "Capacity = len for array-backed buffer; inferable from code"),
    ("aws_byte_buf_from_empty_array","buf.capacity == capacity",     "CODE", "DIRECT",     "LEN_CHANGE",
     "Capacity set to capacity param; slightly redundant but GT asserts it"),
    ("aws_byte_cursor_from_array","cur.len == length",               "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: cursor.len = length"),
    ("aws_byte_cursor_from_buf","cur.len == buf.len",                "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: cursor.len = buf.len"),
    ("aws_byte_cursor_from_c_str","cur.len == strlen(c_str)",        "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: cursor.len = string length"),
    ("aws_byte_buf_init_copy_from_cursor","buf.len == cursor.len",   "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: copy sets len = cursor.len"),
    ("aws_byte_buf_init_copy_from_cursor","buf.capacity == cursor.len","CODE","DIRECT",    "LEN_CHANGE",
     "Capacity = len for exact-fit allocation"),
    ("aws_byte_buf_init_copy_from_cursor","buf.allocator == allocator","NL", "DIRECT",     "LEN_CHANGE",
     "Documented: allocator stored in buf"),
    ("aws_byte_cursor_from_string","cursor.len == str.len",          "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: cursor.len = string.len"),
    ("aws_ring_buffer_acquire","buf.capacity == requested_size",     "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: acquired buffer has capacity = requested_size"),

    # ── CODE / DIRECT or CONDITIONAL: Length invariants / equality checks ─────
    ("aws_array_list_back",   "list.data != null",                   "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT precondition: list must have data before accessing back; LLM misses this guard"),
    ("aws_array_list_back",   "list.length != 0",                    "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT precondition: list non-empty; LLM skips this"),
    ("aws_array_list_get_at", "list.data",                           "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT asserts list.data is valid after get_at"),
    ("aws_array_list_get_at", "list.length > index",                 "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT precondition: index in bounds"),
    ("aws_array_list_front",  "list.data",                           "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT asserts list has data"),
    ("aws_array_list_front",  "list.length",                         "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT asserts list.length non-zero"),
    ("aws_array_list_get_at_ptr","list.data",                        "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT asserts list.data valid"),
    ("aws_array_list_get_at_ptr","list.length > index",              "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT precondition: index in bounds"),
    ("aws_array_list_pop_back","list.data",                          "CODE", "DIRECT",     "LEN_INVARIANT",
     "Data pointer still valid after pop"),
    ("aws_array_list_pop_front","list.data",                         "CODE", "DIRECT",     "LEN_INVARIANT",
     "Data pointer still valid after pop_front"),
    ("aws_array_list_erase",  "index < old.length",                  "CODE", "DIRECT",     "LEN_INVARIANT",
     "GT asserts precondition: index must be < length"),
    ("aws_array_list_swap",   "list.length == old.length",           "CODE", "DIRECT",     "LEN_INVARIANT",
     "Swap doesn't change length"),
    ("aws_array_list_init_dynamic","list.alloc == allocator",        "NL",   "DIRECT",     "LEN_INVARIANT",
     "Documented: allocator stored"),
    ("aws_array_list_init_dynamic","list.current_size == item_size * initial_item_allocation",
     "CODE","DIRECT","LEN_INVARIANT","Allocated size = item_size * count; inferable from code"),
    ("aws_array_list_init_static","list.current_size == initial_item_allocation * item_size",
     "CODE","DIRECT","LEN_INVARIANT","Static list: current_size = allocation*item_size"),

    ("aws_byte_buf_from_c_str","buf.len == strlen(c_str)",           "NL",   "DIRECT",     "LEN_CHANGE",
     "Documented: buf.len = strlen"),
    ("aws_byte_buf_from_c_str","buf.capacity == buf.len",            "CODE", "DIRECT",     "LEN_CHANGE",
     "For c_str wrap, capacity = len (no extra space)"),
    ("aws_byte_buf_from_c_str","buf.allocator == null",              "CODE", "DIRECT",     "RETVAL_SPEC",
     "No allocator for static array wrapper; LLM forgets to assert null allocator"),
    ("aws_byte_buf_from_c_str","strlen(c_str) == 0",                 "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Edge case: empty string; GT tests this branch explicitly"),
    ("aws_byte_buf_from_c_str","buf.len == 0",                       "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Empty string case: buf.len = 0"),
    ("aws_byte_cursor_from_c_str","cur.len == 0",                    "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Edge case: empty c_str"),
    ("aws_byte_buf_secure_zero","buf.len == 0",                      "CODE", "DIRECT",     "LEN_CHANGE",
     "secure_zero clears len; LLM focuses on content zeroing, misses len reset"),
    ("aws_byte_buf_eq_c_str", "buf.len == str_len",                  "CODE", "CONDITIONAL","LEN_INVARIANT",
     "GT saves and asserts string length for comparison; LLM skips intermediate check"),
    ("aws_array_eq",          "lhs_len == rhs_len",                  "CODE", "CONDITIONAL","LEN_INVARIANT",
     "GT asserts length equality condition in comparison logic"),
    ("aws_array_eq_c_str",    "array_len == str_len",                "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Same: length equality for array vs c_str comparison"),
    ("aws_byte_cursor_eq",    "lhs.len == rhs.len",                  "CODE", "CONDITIONAL","LEN_INVARIANT",
     "GT asserts equal-length condition"),
    ("aws_byte_cursor_eq_byte_buf","cur.len == buf.len",             "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Length equality for cursor vs buf comparison"),
    ("aws_byte_cursor_eq_c_str","cur.len == str_len",                "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Length equality check"),
    ("aws_string_eq",         "str_a.len == str_b.len",              "CODE", "CONDITIONAL","LEN_INVARIANT",
     "GT asserts length equality in string comparison"),
    ("aws_string_eq_byte_cursor","str.len == cursor.len",            "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Length equality for string vs cursor"),
    ("aws_string_eq_c_str",   "str.len == strlen(c_str)",            "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Length comparison"),
    ("aws_ring_buffer_init",  "ring_buf.allocation_end - ring_buf.allocation == size",
     "CODE","DIRECT","LEN_CHANGE","Allocation span = requested size; requires pointer arithmetic understanding"),
    ("aws_ring_buffer_acquire","buf.len == 0",                       "CODE", "DIRECT",     "LEN_INVARIANT",
     "Newly acquired buffer starts empty (len=0)"),

    # ── CODE / DIRECT: Return value specifications ────────────────────────────
    ("aws_ptr_eq",            "rval == (p1 == p2)",                  "CODE", "DIRECT",     "RETVAL_SPEC",
     "Return value: 1 iff pointers equal; trivial but LLM writes verbose if/else instead"),
    ("aws_string_bytes",      "aws_string_bytes(str) == str.bytes",  "CODE", "DIRECT",     "RETVAL_SPEC",
     "Accessor: return value IS the bytes field; LLM may assert content not pointer"),
    ("aws_ring_buffer_buf_belongs_to_pool","is_member == result",    "CODE", "DIRECT",     "RETVAL_SPEC",
     "Return value semantics: result matches pre-computed membership"),

    # ── CODE / DIRECT: Cursor advance postconditions ──────────────────────────
    ("aws_byte_cursor_advance","rv.ptr == null",                     "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Failure path: returned cursor ptr is null; LLM misses failure case"),
    ("aws_byte_cursor_advance","rv.len == 0",                        "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Failure path: returned cursor len is 0"),
    ("aws_byte_cursor_advance","rv.ptr == old.ptr",                  "CODE", "CONDITIONAL","LEN_INVARIANT",
     "Success path: returned cursor starts at original position"),
    ("aws_byte_cursor_advance","rv.len == len",                      "NL",   "DIRECT",     "LEN_CHANGE",
     "Success: returned cursor length = requested len"),
    ("aws_byte_cursor_advance","cursor.ptr == old.ptr + len",        "CODE", "DIRECT",     "STRUCT_PTR",
     "Cursor pointer advances by len; pointer arithmetic postcondition"),

    # ── CODE / DIRECT: Value match for front element ──────────────────────────
    ("aws_array_list_front",  "aws_bytes_eq(val, list.data, list.item_size)","CODE","DIRECT","VALIDITY_PRED",
     "GT uses aws_bytes_eq to check first element value; LLM uses memcmp or doesn't check"),

    # ── Zero checks ───────────────────────────────────────────────────────────
    ("aws_array_list_clean_up","aws_is_zeroed(list)",                "DOMAIN","STRUCTURAL","ZERO_CHECK",
     "GT checks entire list is zeroed after clean_up; uses AWS proof-helper predicate"),
    ("aws_linked_list_node_reset","aws_is_zeroed(node)",             "DOMAIN","STRUCTURAL","ZERO_CHECK",
     "GT checks node is zeroed; uses aws_is_zeroed predicate"),
    ("aws_array_list_init_dynamic","aws_is_zeroed(list)",            "DOMAIN","STRUCTURAL","ZERO_CHECK",
     "On init failure, list should be zeroed; GT checks this via aws_is_zeroed"),
    ("aws_string_destroy_secure","old_str.bytes[i] == 0",           "CODE",  "DIRECT",     "ZERO_CHECK",
     "Each byte of the string is zeroed; LLM may assert destruction but not byte-level zeroing"),
    ("aws_ring_buffer_clean_up","aws_atomic_load_ptr(&ring_buf.head) == null","CODE","DIRECT","ZERO_CHECK",
     "Head atomic pointer set to null after cleanup"),
    ("aws_ring_buffer_clean_up","aws_atomic_load_ptr(&ring_buf.tail) == null","CODE","DIRECT","ZERO_CHECK",
     "Tail atomic pointer set to null after cleanup"),

    # ── DOMAIN / STRUCTURAL: Memory predicates ────────────────────────────────
    ("aws_ring_buffer_acquire","aws_mem_is_writable",                "DOMAIN","STRUCTURAL","CBMC_BUILTIN",
     "CBMC memory predicate: acquired buffer memory is writable"),

    # ── Precondition instances ────────────────────────────────────────────────
    ("aws_array_list_swap",   "!list.length || list.data check failed","CODE","CONDITIONAL","LEN_INVARIANT",
     "GT precondition_instance: list must be valid before swap; LLM misses this guard"),

    # ── String length bounds ──────────────────────────────────────────────────
    ("aws_string_new_from_c_str","str.len <= max_string_len",        "CODE","CONDITIONAL","LEN_INVARIANT",
     "GT bounds string length to MAX_STRING_LEN (CBMC unwind bound); LLM doesn't know this bound"),
    ("aws_string_new_from_c_str","str.bytes[str.len] == 0",          "CODE","DIRECT",      "LEN_INVARIANT",
     "Null termination invariant for aws_string"),
]


def find_annotation(func: str, prop: str):
    """Find annotation for a property. Try func-specific first, then generic."""
    prop_lower = prop.lower()
    # Try func-specific match first
    for (f, frag, info, reason, cat, note) in ANNOTATIONS:
        if f == func and frag.lower() in prop_lower:
            return info, reason, cat, note
    # Then try generic (f is None)
    for (f, frag, info, reason, cat, note) in ANNOTATIONS:
        if f is None and frag.lower() in prop_lower:
            return info, reason, cat, note
    return "UNCLASSIFIED", "UNCLASSIFIED", "UNCLASSIFIED", ""


def main():
    with open(RESULTS / 'cross_verify_results_condA_claude.json') as f:
        data = json.load(f)

    rows = []
    prop_id = 1
    unclassified = []

    for entry in data:
        func = entry['func']
        missed = entry.get('unmatched_gt_harness', [])
        if not missed:
            continue

        gt_dirs = list(DATASET.glob(f'func*_{func}'))
        gt_snippet = ''
        if gt_dirs:
            gt_file = gt_dirs[0] / 'ground_truth_harness.c'
            if gt_file.exists():
                gt_snippet = gt_file.read_text().strip()[:600]

        for prop in missed:
            prop_text, prop_type = prop.rsplit('|', 1)
            prop_text = prop_text.strip()
            info, reason, cat, note = find_annotation(func, prop_text)

            if info == "UNCLASSIFIED":
                unclassified.append((func, prop_text))

            rows.append({
                'id': f'P{prop_id:03d}',
                'func': func,
                'property_text': prop_text,
                'property_kind': prop_type,
                'info_source': info,
                'reasoning': reason,
                'category': cat,
                'annotation_note': note,
                'gt_snippet': gt_snippet,
            })
            prop_id += 1

    # Write annotated CSV
    with open(OUT_DIR / 'annotated_missed_properties.csv', 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)

    print(f"Written {len(rows)} annotated rows")
    print(f"Unclassified: {len(unclassified)}")
    if unclassified:
        print("\nUnclassified properties:")
        for func, prop in unclassified:
            print(f"  {func}: {prop}")

    # Summary statistics
    from collections import Counter
    info_counts = Counter(r['info_source'] for r in rows)
    reason_counts = Counter(r['reasoning'] for r in rows)
    cat_counts = Counter(r['category'] for r in rows)

    print("\n── Info Source ──")
    for k, v in sorted(info_counts.items(), key=lambda x: -x[1]):
        print(f"  {k:15s}: {v:3d}  ({100*v/len(rows):.1f}%)")

    print("\n── Reasoning Complexity ──")
    for k, v in sorted(reason_counts.items(), key=lambda x: -x[1]):
        print(f"  {k:15s}: {v:3d}  ({100*v/len(rows):.1f}%)")

    print("\n── Category ──")
    for k, v in sorted(cat_counts.items(), key=lambda x: -x[1]):
        print(f"  {k:20s}: {v:3d}  ({100*v/len(rows):.1f}%)")

    print("\n── 2D Matrix (info_source × reasoning) ──")
    matrix = Counter((r['info_source'], r['reasoning']) for r in rows)
    for (info, reason), cnt in sorted(matrix.items()):
        print(f"  {info:10s} × {reason:15s}: {cnt:3d}")


if __name__ == '__main__':
    main()
