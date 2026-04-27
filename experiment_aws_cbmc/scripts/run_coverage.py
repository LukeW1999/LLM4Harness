#!/usr/bin/env python3
"""
run_coverage.py
===============
Run CBMC branch coverage on GT and LLM harnesses to quantify over-constraining.

Workflow
--------
For each function:
  1. Run GT harness  with --cover branch → B_GT  (benchmark)
  2. Run LLM harness with --cover branch → B_LLM
  3. coverage_ratio = |B_LLM ∩ B_GT| / |B_GT|   (< 1.0 means over-constraining)

Branch IDs are anchored to the TARGET FUNCTION ONLY (not harness, not stubs),
identified by (file, line, branch-direction) in the CBMC XML output.

Usage
-----
  # GT only (build the benchmark)
  python3 run_coverage.py --gt-only

  # Full comparison (one variant, one iteration)
  python3 run_coverage.py --variant feedback_loop_A_claude --iter 1

  # Per-iteration comparison (all 4 rounds, one variant)
  python3 run_coverage.py --variant feedback_loop_A_claude --all-iters

  # All variants × all iterations
  python3 run_coverage.py --all
"""

import argparse
import json
import subprocess
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

# ── paths ────────────────────────────────────────────────────────────────────
SCRIPT_DIR  = Path(__file__).parent
PROJECT_DIR = SCRIPT_DIR.parent
DATASET_DIR = PROJECT_DIR / "dataset"
RESULTS_DIR = PROJECT_DIR / "results"
SRCDIR      = Path("/home/weiqi/Verification/aws-c-common")
PROOFDIR    = SRCDIR / "verification/cbmc"

sys.path.insert(0, str(SCRIPT_DIR))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS

# ── data structures ───────────────────────────────────────────────────────────
@dataclass
class BranchSet:
    """Branches in the target function reachable under a given harness."""
    func_name: str
    satisfied: set = field(default_factory=set)   # reachable branches
    failed:    set = field(default_factory=set)   # blocked branches

    @property
    def total(self) -> int:
        return len(self.satisfied) + len(self.failed)

    @property
    def coverage_count(self) -> int:
        return len(self.satisfied)

    def coverage_pct(self) -> float:
        return self.coverage_count / self.total if self.total else 0.0


@dataclass
class ComparisonResult:
    func_name:        str
    gt_covered:       int
    gt_total:         int
    llm_covered:      int
    llm_total:        int
    intersection:     int          # branches covered by BOTH
    coverage_ratio:   Optional[float]   # intersection / gt_covered
    over_constrained: bool         # llm_covered < gt_covered
    missed_branches:  list         # branches in GT but not LLM

    def to_dict(self) -> dict:
        return {
            "func_name":        self.func_name,
            "gt_covered":       self.gt_covered,
            "gt_total":         self.gt_total,
            "llm_covered":      self.llm_covered,
            "llm_total":        self.llm_total,
            "intersection":     self.intersection,
            "coverage_ratio":   round(self.coverage_ratio, 4)
                                if self.coverage_ratio is not None else None,
            "over_constrained": self.over_constrained,
            "missed_count":     len(self.missed_branches),
        }


# ── CBMC invocation ───────────────────────────────────────────────────────────
def build_cbmc_cmd(func_name: str, harness_path: Path) -> list:
    cfg = FUNC_CONFIGS[func_name]
    return (
        ["cbmc", "--cover", "branch", "--xml-ui"]
        + COMMON_FLAGS
        + cfg.get("unwind", [])
        + cfg.get("unwindset", [])
        + cfg.get("defines", [])
        + [str(p) for p in cfg.get("project_sources", [])]
        + [str(p) for p in cfg.get("proof_sources", [])]
        + [str(harness_path)]
        + ["--function", f"{func_name}_harness"]
    )


def run_coverage(func_name: str, harness_path: Path,
                 timeout: int = 300) -> Optional[BranchSet]:
    """
    Run CBMC --cover branch and return branch coverage anchored to the
    target function only (not the harness wrapper, not stubs).
    Returns None on timeout or parse failure.
    """
    if func_name not in FUNC_CONFIGS:
        print(f"    [SKIP] {func_name} not in FUNC_CONFIGS")
        return None

    cmd = build_cbmc_cmd(func_name, harness_path)

    try:
        proc = subprocess.run(
            cmd, capture_output=True, text=True, timeout=timeout
        )
        xml_text = proc.stdout
    except subprocess.TimeoutExpired:
        print(f"    [TIMEOUT] {func_name} (>{timeout}s)")
        return None
    except Exception as e:
        print(f"    [ERROR] {func_name}: {e}")
        return None

    # CBMC exit code 6 = internal error (e.g. too many addressed objects).
    # In that case stdout XML has an <ERROR> message and no <goal> elements.
    if proc.returncode == 6 or "too many addressed objects" in xml_text:
        print(f"    [CBMC-ERROR rc={proc.returncode}] {func_name}", end=" ")
        return None

    bs = parse_xml(func_name, xml_text)
    # An empty BranchSet (0 satisfied, 0 failed) after a successful CBMC run
    # is legitimate only if the function truly has no branch coverage goals.
    # Guard: if CBMC exited with an error code, discard the result.
    if bs is not None and bs.total == 0 and proc.returncode not in (0, 10):
        return None
    return bs


def parse_xml(func_name: str, xml_text: str,
              exclude_files: Optional[set] = None) -> Optional[BranchSet]:
    """
    Parse CBMC --cover branch XML output.

    Goal elements look like:
      <goal description="..." status="SATISFIED|FAILED">
        <location file="..." function="..." line="..." column="..."/>
      </goal>

    We collect ALL branches across the entire CBMC run (no function-name
    filter), because many target functions are static-inline and their
    branches are attributed to the calling harness function by CBMC.

    The coverage ratio GT∩LLM/GT is still valid:
      - Infrastructure branches (aws_common_library_init etc.) are FAILED
        in both runs → they never appear in satisfied sets → no bias.
      - Over-constraining shows up as LLM missing satisfied branches that
        GT covers (e.g. the u32 path in aws_add_size_checked).

    exclude_files: basenames to skip (e.g. stubs, make_common_data_structures.c).
    Default exclusion list covers known proof infrastructure.
    """
    if not xml_text.strip():
        return None

    try:
        root = ET.fromstring(xml_text)
    except ET.ParseError:
        # CBMC sometimes mixes stdout/stderr; try to extract just the XML
        start = xml_text.find("<?xml")
        if start == -1:
            return None
        try:
            root = ET.fromstring(xml_text[start:])
        except ET.ParseError:
            return None

    # Default: exclude proof-infrastructure files that both harnesses share.
    # These are all FAILED anyway so wouldn't affect the ratio, but
    # excluding them keeps branch IDs cleaner and sets smaller.
    if exclude_files is None:
        exclude_files = {
            "make_common_data_structures.c",
            "error.c",
        }

    result = BranchSet(func_name=func_name)

    for goal in root.iter("goal"):
        status = goal.get("status", "")
        loc    = goal.find("location")
        if loc is None:
            continue

        fname = Path(loc.get("file", "")).name  # basename only
        if fname in exclude_files:
            continue

        branch_id = "{}:{}:{}:{}".format(
            fname,
            loc.get("function", "?"),
            loc.get("line", "?"),
            goal.get("description", "?"),
        )

        if status == "SATISFIED":
            result.satisfied.add(branch_id)
        else:
            result.failed.add(branch_id)

    return result


# ── comparison ────────────────────────────────────────────────────────────────
def compare(gt: BranchSet, llm: BranchSet) -> ComparisonResult:
    inter = gt.satisfied & llm.satisfied
    ratio = len(inter) / len(gt.satisfied) if gt.satisfied else None
    missed = sorted(gt.satisfied - llm.satisfied)

    return ComparisonResult(
        func_name        = gt.func_name,
        gt_covered       = len(gt.satisfied),
        gt_total         = gt.total,
        llm_covered      = len(llm.satisfied),
        llm_total        = llm.total,
        intersection     = len(inter),
        coverage_ratio   = ratio,
        over_constrained = len(llm.satisfied) < len(gt.satisfied),
        missed_branches  = missed,
    )


# ── helpers ───────────────────────────────────────────────────────────────────
def find_gt_harness(func_name: str) -> Optional[Path]:
    for d in DATASET_DIR.iterdir():
        if d.is_dir() and d.name.endswith(f"_{func_name}"):
            h = d / "ground_truth_harness.c"
            if h.exists():
                return h
    return None


def find_llm_harness(func_name: str, variant: str, iteration: int) -> Optional[Path]:
    p = RESULTS_DIR / variant / func_name / f"iter_{iteration}_harness.c"
    return p if p.exists() else None


def eligible_funcs() -> list:
    """Functions that have both a GT harness and a FUNC_CONFIGS entry."""
    funcs = []
    for d in sorted(DATASET_DIR.iterdir()):
        if not d.is_dir():
            continue
        parts = d.name.split("_", 1)
        if len(parts) < 2:
            continue
        fname = parts[1]
        if fname in FUNC_CONFIGS and (d / "ground_truth_harness.c").exists():
            funcs.append(fname)
    return funcs


def print_summary(results: list[ComparisonResult]):
    valid = [r for r in results if r.coverage_ratio is not None]
    if not valid:
        print("No valid comparisons.")
        return
    avg = sum(r.coverage_ratio for r in valid) / len(valid)
    oc  = sum(1 for r in valid if r.over_constrained)
    print(f"\n{'='*60}")
    print(f"  Functions analyzed : {len(valid)}")
    print(f"  Avg coverage ratio : {avg:.1%}  (LLM / GT reachable branches)")
    print(f"  Over-constrained   : {oc}/{len(valid)}  ({oc/len(valid):.0%})")
    print(f"{'='*60}")


# ── GT benchmark ─────────────────────────────────────────────────────────────
def _save_benchmark(benchmark: dict, output_path: Path):
    """Serialise and write benchmark dict to JSON (overwrites in place)."""
    serialisable = {
        fn: {
            "satisfied": sorted(bs.satisfied),
            "failed":    sorted(bs.failed),
        }
        for fn, bs in benchmark.items()
    }
    output_path.write_text(json.dumps(serialisable, indent=2))


def build_gt_benchmark(funcs: list, timeout: int = 300,
                       existing: dict | None = None) -> dict:
    """
    Run GT harnesses with --cover branch and save results.
    Saves incrementally after each function so progress is not lost on
    interruption.  Pass existing={} or a loaded cache as ``existing`` to
    merge freshly computed results into it.
    Returns {func_name: BranchSet}.
    """
    benchmark: dict = dict(existing) if existing else {}
    output_path = RESULTS_DIR / "gt_branch_coverage.json"

    for func_name in funcs:
        gt_harness = find_gt_harness(func_name)
        if gt_harness is None:
            print(f"  [{func_name}] no GT harness")
            continue

        print(f"  [{func_name}] running GT coverage ...", end="", flush=True)
        bs = run_coverage(func_name, gt_harness, timeout=timeout)

        if bs is None:
            print(" FAILED")
            continue

        print(f" {bs.coverage_count}/{bs.total} ({bs.coverage_pct():.0%})")
        benchmark[func_name] = bs
        _save_benchmark(benchmark, output_path)   # incremental save

    print(f"\nGT benchmark saved → {output_path}")
    return benchmark


def load_gt_benchmark() -> dict:
    """Load a previously saved GT benchmark. Returns {func_name: BranchSet}."""
    p = RESULTS_DIR / "gt_branch_coverage.json"
    if not p.exists():
        return {}
    data = json.loads(p.read_text())
    result = {}
    for fn, d in data.items():
        bs = BranchSet(func_name=fn)
        bs.satisfied = set(d["satisfied"])
        bs.failed    = set(d["failed"])
        result[fn] = bs
    return result


# ── comparison runs ───────────────────────────────────────────────────────────
def run_comparison(variant: str, iteration: int,
                   gt_benchmark: dict,
                   funcs: list,
                   timeout: int = 300) -> list[ComparisonResult]:
    results = []
    for func_name in funcs:
        gt_bs = gt_benchmark.get(func_name)
        if gt_bs is None:
            continue

        llm_harness = find_llm_harness(func_name, variant, iteration)
        if llm_harness is None:
            print(f"  [{func_name}] no LLM harness (variant={variant}, iter={iteration})")
            continue

        print(f"  [{func_name}] LLM iter {iteration} ...", end="", flush=True)
        llm_bs = run_coverage(func_name, llm_harness, timeout=timeout)

        if llm_bs is None:
            print(" FAILED")
            continue

        cmp = compare(gt_bs, llm_bs)
        ratio_str = f"{cmp.coverage_ratio:.1%}" if cmp.coverage_ratio is not None else "N/A"
        oc_str    = " [OVER-CONSTRAINED]" if cmp.over_constrained else ""
        print(f" GT {cmp.gt_covered} | LLM {cmp.llm_covered} | ratio {ratio_str}{oc_str}")
        results.append(cmp)

    return results


# ── main ─────────────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--gt-only", action="store_true",
                        help="Build GT benchmark only, no LLM comparison")
    parser.add_argument("--variant", default="feedback_loop_A_claude",
                        help="LLM result variant subdirectory name")
    parser.add_argument("--iter", type=int, default=1,
                        help="Harness iteration to compare (1–4)")
    parser.add_argument("--all-iters", action="store_true",
                        help="Run all 4 iterations for the given variant")
    parser.add_argument("--all", action="store_true",
                        help="Run all variants × all iterations")
    parser.add_argument("--timeout", type=int, default=300,
                        help="CBMC timeout per function in seconds")
    parser.add_argument("--funcs", nargs="+",
                        help="Only process these functions (optional filter)")
    args = parser.parse_args()

    funcs = eligible_funcs()
    if args.funcs:
        funcs = [f for f in funcs if f in args.funcs]
    print(f"Functions to process: {len(funcs)}")

    # ── step 1: GT benchmark ─────────────────────────────────────────────────
    cached = load_gt_benchmark()
    missing_gt = [f for f in funcs if f not in cached]

    if missing_gt or args.gt_only:
        to_run = missing_gt if missing_gt else funcs
        print(f"\n[Building GT benchmark for {len(to_run)} functions]")
        fresh = build_gt_benchmark(to_run, timeout=args.timeout, existing=cached)
        cached.update(fresh)

    gt_benchmark = {fn: cached[fn] for fn in funcs if fn in cached}

    if args.gt_only:
        return

    # ── step 2: LLM comparison ───────────────────────────────────────────────
    all_variants = [
        d.name for d in RESULTS_DIR.iterdir()
        if d.is_dir() and d.name.startswith("feedback_loop")
    ]

    variants  = all_variants          if args.all else [args.variant]
    iters     = list(range(1, 5))     if (args.all or args.all_iters) else [args.iter]

    all_results = {}   # (variant, iter) → [ComparisonResult]

    for variant in variants:
        for it in iters:
            key = (variant, it)
            print(f"\n[{variant}  iter={it}]")
            res = run_comparison(variant, it, gt_benchmark, funcs,
                                 timeout=args.timeout)
            all_results[key] = res
            print_summary(res)

    # ── save ─────────────────────────────────────────────────────────────────
    save_path = RESULTS_DIR / "coverage_comparison.json"
    serialisable = {
        f"{v}_iter{i}": [r.to_dict() for r in rs]
        for (v, i), rs in all_results.items()
    }
    save_path.write_text(json.dumps(serialisable, indent=2))
    print(f"\nFull results saved → {save_path}")


if __name__ == "__main__":
    main()
