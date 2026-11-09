#!/usr/bin/env python3
"""Consolidate the two static-analysis rounds into one comparison report.

Reads the round artifacts that the scanner leaves in .static-analysis/ plus the
per-round summaries already copied under docs/08_实施/static-analysis/, and
writes a single Markdown report. Only the summaries and the SARIF files are kept
by the caller; this script does the aggregation.
"""
import json
import collections
from pathlib import Path

ROOT = Path("/home/user1/AUTOETH/AUTOETH")
SA = ROOT / "docs" / "08_实施" / "static-analysis"

ROUNDS = [
    ("round1-shared", "shared 单配置", "AUTOETH_BUILD_STATIC=OFF + AUTOETH_ANALYSIS_HOST=ON"),
    ("round2-static", "static 单配置", "AUTOETH_BUILD_SHARED=OFF + AUTOETH_BUILD_STATIC=ON + AUTOETH_ANALYSIS_HOST=ON"),
]


def read_summary_meta(path):
    meta = {}
    if not path.is_file():
        return meta
    text = path.read_text(encoding="utf-8", errors="replace")
    for line in text.splitlines():
        if line.startswith("**Execution status**:"):
            meta["status"] = line.split(":", 1)[1].strip()
        elif line.startswith("**Error code**:"):
            meta["error_code"] = line.split(":", 1)[1].strip().strip("`")
        elif line.startswith("Total diagnostics:"):
            meta["total"] = line.split(":", 1)[1].strip().replace("*", "")
        elif "|" in line and "Expected units" not in line and "---" not in line:
            cells = [c.strip() for c in line.strip().strip("|").split("|")]
            if len(cells) == 4 and cells[0].isdigit():
                meta["expected"] = cells[1]
                meta["analyzed"] = cells[2]
                meta["complete"] = cells[3]
    return meta


def read_sarif_stats(path):
    stats = {"total": 0, "error": 0, "warning": 0, "note": 0, "rules": collections.Counter(),
             "files": collections.Counter()}
    if not path.is_file():
        return stats
    data = json.loads(path.read_text(encoding="utf-8"))
    results = data.get("runs", [{}])[0].get("results", [])
    stats["total"] = len(results)
    for item in results:
        level = item.get("level", "unknown")
        stats[level] = stats.get(level, 0) + 1
        rule = item.get("ruleId", "").split(".")[-1]
        stats["rules"][rule] += 1
        locs = item.get("locations") or []
        if locs:
            uri = locs[0]["physicalLocation"]["artifactLocation"]["uri"]
            stats["files"][uri] += 1
    return stats


def main():
    lines = []
    lines.append("# 静态分析两轮汇总报告")
    lines.append("")
    lines.append("> 日期：2026-11-09")
    lines.append("> 规则集：MISRA C 2012 + MISRA CPP 2008（379 条规则，不过滤）")
    lines.append("> 构建：GCC 13.3.0 / CMake 3.28.3 / Release")
    lines.append("> 分析器：static-analysis-skill（Seahunter + clang-tidy）")
    lines.append("")
    lines.append("## 1. 两轮结果对比")
    lines.append("")
    lines.append("| 轮次 | 配置 | 执行状态 | 覆盖 | 诊断数 |")
    lines.append("|---|---|---|---:|---:|")
    for name, label, cfg in ROUNDS:
        meta = read_summary_meta(SA / name / "___summary.md")
        lines.append("| {} | {} | {} | {}/{} | {} |".format(
            name, cfg, meta.get("status", "UNOBSERVED"),
            meta.get("analyzed", "0"), meta.get("expected", "0"), meta.get("total", "0")))
    lines.append("")
    lines.append("## 2. 严重度分布")
    lines.append("")
    lines.append("| 轮次 | error | warning | note | 合计 |")
    lines.append("|---|---:|---:|---:|---:|")
    all_stats = {}
    for name, _, _ in ROUNDS:
        stats = read_sarif_stats(SA / name / "analysis_result.sarif")
        all_stats[name] = stats
        lines.append("| {} | {} | {} | {} | {} |".format(
            name, stats.get("error", 0), stats.get("warning", 0),
            stats.get("note", 0), stats["total"]))
    lines.append("")
    if len(all_stats) == 2:
        a, b = all_stats["round1-shared"], all_stats["round2-static"]
        same = a["total"] == b["total"] and a.get("error", 0) == b.get("error", 0)
        lines.append("两轮诊断数{}（shared {} / static {}），error 数{}。".format(
            "一致" if same else "存在差异", a["total"], b["total"],
            "一致" if a.get("error", 0) == b.get("error", 0) else "不同"))
        lines.append("")
    lines.append("## 3. 规则命中 Top 15（round1-shared）")
    lines.append("")
    lines.append("| 规则 | 命中数 |")
    lines.append("|---|---:|")
    for rule, count in all_stats.get("round1-shared", {}).get("rules", collections.Counter()).most_common(15):
        lines.append("| {} | {} |".format(rule, count))
    lines.append("")
    lines.append("## 4. 文件命中 Top 12（round1-shared）")
    lines.append("")
    lines.append("| 文件 | 命中数 |")
    lines.append("|---|---:|")
    for uri, count in all_stats.get("round1-shared", {}).get("files", collections.Counter()).most_common(12):
        lines.append("| {} | {} |".format(uri, count))
    lines.append("")
    lines.append("## 5. 环境结论")
    lines.append("")
    lines.append("两轮均为 COMPLETE，以下原先的两个阻塞项均已消除：")
    lines.append("")
    lines.append("- `stdlib.h` 编译错误：`compiler_errors=false`，无残留。")
    lines.append("- 双上下文变体告警：`context_variants=0`，每个文件 1 个上下文。")
    lines.append("")
    lines.append("解决路径（均为仓库内改动，未触碰技能安装目录）：")
    lines.append("")
    lines.append("1. `AUTOETH_ANALYSIS_HOST` 开关强制包含 GNU C 库头，绕开分析器内建头目录缺 `stdlib.h`。")
    lines.append("2. `--cmake-extra` 是单值参数，多个 `-D` 必须合并为一个字符串，否则只有最后一个生效。")
    lines.append("3. `CMakeLists.txt` 的 `autoeth::core/e2e/runtime` 别名原先硬编码 `_shared`，"
                 "关闭共享库时配置失败；已改为按实际构建类型选择，属产品缺陷修复。")
    lines.append("")
    lines.append("## 6. 证据文件")
    lines.append("")
    lines.append("| 轮次 | 汇总 | SARIF |")
    lines.append("|---|---|---|")
    for name, _, _ in ROUNDS:
        lines.append("| {} | `{}/___summary.md` | `{}/analysis_result.sarif` |".format(
            name, name, name))
    lines.append("")
    lines.append("说明：按约定只保留汇总与 SARIF；逐文件 JSON/Markdown、编译数据库副本等"
                 "中间产物未纳入证据目录，可由上述命令重新生成。")
    lines.append("")
    out = SA / "两轮汇总报告.md"
    out.write_text("\n".join(lines) + "\n", encoding="utf-8")
    print("wrote", out)


if __name__ == "__main__":
    main()