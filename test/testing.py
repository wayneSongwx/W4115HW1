import os
import re


def read_file_lines(filepath):
    with open(filepath, 'r') as f:
        return f.read().splitlines()


def parse_out_line(line):
    m = re.match(r"(\w+)\('([^']*)'\) at (\d+):(\d+)", line)
    if m:
        token_type, token_value, line_num, col_num = m.groups()
        return token_type, token_value, int(line_num), int(col_num)
    return None


def parse_err_line(line):
    m = re.search(r"line (\d+), column (\d+)", line)
    if m:
        return int(m.group(1)), int(m.group(2))
    return None


def score_out_files(file1, file2):
    lines1 = read_file_lines(file1) if os.path.exists(file1) else []
    lines2 = read_file_lines(file2) if os.path.exists(file2) else []

    max_lines = max(len(lines1), len(lines2))
    total_score = 0
    max_score = max_lines * 4  # 4 criteria per token line

    for i in range(max_lines):
        if i < len(lines1):
            parsed1 = parse_out_line(lines1[i])
        else:
            parsed1 = None

        if i < len(lines2):
            parsed2 = parse_out_line(lines2[i])
        else:
            parsed2 = None

        if not parsed1 or not parsed2:
            # Skip line if parsing fails on either side
            continue

        total_score += (parsed1[0] == parsed2[0])  # Token Type
        total_score += (parsed1[1] == parsed2[1])  # Token Value
        total_score += (parsed1[2] == parsed2[2])  # Line Number
        total_score += (parsed1[3] == parsed2[3])  # Column Number

    return total_score, max_score


def score_err_files(file1, file2):
    lines1 = read_file_lines(file1) if os.path.exists(file1) else []
    lines2 = read_file_lines(file2) if os.path.exists(file2) else []

    max_lines = max(len(lines1), len(lines2))
    total_score = 0
    max_score = max_lines * 2  # line + col number per error line

    for i in range(max_lines):
        if i < len(lines1):
            parsed1 = parse_err_line(lines1[i])
        else:
            parsed1 = None

        if i < len(lines2):
            parsed2 = parse_err_line(lines2[i])
        else:
            parsed2 = None

        if not parsed1 or not parsed2:
            continue

        total_score += (parsed1[0] == parsed2[0])  # Line Number
        total_score += (parsed1[1] == parsed2[1])  # Column Number

    return total_score, max_score


def combine_scores(dir1, dir2, dir3):
    out_files = {f[:-4] for f in os.listdir(dir1) if f.endswith('.out')}
    err_files = {f[:-4] for f in os.listdir(dir1) if f.endswith('.err')}
    common_files = out_files & err_files

    results = []

    for base in sorted(common_files):
        out1 = os.path.join(dir1, base + '.out')
        out2 = os.path.join(dir2, base + '.out')
        err1 = os.path.join(dir1, base + '.err')
        err2 = os.path.join(dir3, base + '.err')

        out_score, out_max = score_out_files(out1, out2)
        err_score, err_max = score_err_files(err1, err2)

        total_score = out_score + err_score
        total_max = out_max + err_max

        results.append((base, total_score, total_max, out_score, err_score))

    return results


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Compare and score .out and .err files together")
    parser.add_argument("dir1", help="First directory path")
    parser.add_argument("dir2", help="Second directory path")
    parser.add_argument("dir3", help="Third directory path")
    args = parser.parse_args()

    combined_results = combine_scores(args.dir1, args.dir2, args.dir3)

    print(f"{'Filename':<50} {'Total Score':>11} {'Max Score':>10} {'Standard Output Score':>22} {'Standard Error Score':>21}")
    print("-" * 120)
    for base, total_score, total_max, out_score, err_score in combined_results:
        print(f"{base:<50} {total_score:>11} {total_max:>10} {out_score:>22} {err_score:>21}")
