# Archived scripts

Superseded / one-off scripts kept for provenance. **Nothing in the live pipeline
imports these** (verified 2026-06-16), and they are NOT part of reproduction —
see `../../REPRODUCE.md`.

| File | Why archived |
|------|--------------|
| `feedback_loop_TOPLEVEL_STALE.py` | Stale 918-line copy that sat at the repo root; the live generator is `scripts/feedback_loop.py` (1219 lines). Kept to avoid confusion over which to run. |
| `recompute_sac.py`, `recompute_sac2.py` | One-off SAC recomputations, superseded by the registered figures in `paper_numbers.py`. |
| `rebuild_t7.py` | One-off Table 7 rebuild; the table is now produced/audited via `paper_numbers.py` + `attribution_v2.py`. |
| `compare_ABCD.py`, `compare_AEF.py` | Early condition-comparison probes, superseded by `compare_conditions.py`. |
| `v2_all.py`, `v2_extra.py` | Tiny batch wrappers from an earlier iteration. |
