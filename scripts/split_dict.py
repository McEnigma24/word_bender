#!/usr/bin/env python3
"""Split input/dict.txt into dict_<letter>.txt buckets, shortest words first."""

from __future__ import annotations

import unicodedata
from collections import defaultdict
from pathlib import Path


def utf8_first_grapheme(word: str) -> str:
    if not word:
        return ""
    return word[0]


def bucket_letter(word: str) -> str:
    first = utf8_first_grapheme(word)
    return unicodedata.normalize("NFC", first.casefold())


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    input_dir = root / "input"
    source = input_dir / "dict.txt"

    if not source.is_file():
        raise SystemExit(f"Missing {source}")

    buckets: dict[str, set[str]] = defaultdict(set)

    with source.open(encoding="utf-8") as handle:
        for line in handle:
            for part in line.split(","):
                word = part.strip()
                if not word:
                    continue
                buckets[bucket_letter(word)].add(word)

    for path in input_dir.glob("dict_*.txt"):
        if path.name == "dict.txt":
            continue
        path.unlink()

    for letter in sorted(buckets.keys()):
        words = sorted(buckets[letter], key=lambda w: (len(w), w.casefold()))
        out_path = input_dir / f"dict_{letter}.txt"
        out_path.write_text("\n".join(words) + "\n", encoding="utf-8")
        print(f"{out_path.name}: {len(words)} words")


if __name__ == "__main__":
    main()
