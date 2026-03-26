# C++ Module 09 レビュー結果（日本語）

## 共通ルール
- コンパイル: `-Wall -Wextra -Werror -std=c++98` を各 Makefile で使用。OK
- 禁止事項: `using namespace`/`friend`/C系 `*printf/*alloc/free` は未使用。OK
- OCF: 各クラスでデフォルト/コピー/代入/デストラクタを実装または非公開化。OK
- ヘッダ: `#pragma once` によるガード、実装はソース側（テンプレート除く）。OK
- メモリリーク: 実装上のリーク要因は見当たらず。OK
- Makefile: `NAME/all/clean/fclean/re` を備え、不要な再リンクも発生しにくい構成。OK

## モジュール固有ルール
- コンテナ利用: 各課題で少なくとも1つのSTLコンテナを使用。OK
- ex02 は異なる2種類（`vector` と `deque`）を使用。OK
- 再利用制約: ex00の `map` は他課題で未使用、ex01は `stack`（内部 `list`）を使用、ex02 は `vector`/`deque`。OK
- deque/stackの注意: ex01 は `std::stack` のデフォルト `deque` を避け、`list` を指定する形で実装。OK

---

## Exercise 00: Bitcoin Exchange
- バイナリ名: `btc`。OK
- 引数なし時: `Error: could not open file.` を標準エラーに出力。OK
- 入力形式: `date | value` をトリムしつつ正しくパース。OK
- 値検証: 負値→`Error: not a positive number.`、1000超→`Error: too large a number.`、数値（整数/浮動小数）解釈。OK
- 日付検証: `YYYY-MM-DD` を検査し、`tm`+`mktime` 正規化で妥当性確認。OK
  - うるう年: ルール（4で割れ100で割れず、または400で割れる年）に沿って検証。
    - 許可: `2024-02-29`, `2016-02-29`, `2000-02-29`
    - 否定: `2025-02-29`, `2100-02-29`
  - DB最古以前の日付はエラー扱い（`Error: bad input => <line>`）。OK（テスター仕様に合致）
- DB検索: `std::map` と `upper_bound` で直前（下側）の日付レートを使用。OK
- 出力: `YYYY-MM-DD => value = result` 形式で計算結果を表示。OK

補足（任意改善）:
- `value` のパースで末尾まで消費確認（例: `1a` を弾く）を行うと堅牢性が上がります。

---

## Exercise 01: Reverse Polish Notation
- バイナリ名: `RPN`。OK
- 入力: コマンドライン引数1個で逆ポーランド式。OK
- 演算子: `+`, `-`, `*`, `/`。OK
- エラーハンドリング:
  - 括弧や不正トークン→`Error: invalid token: ...`
  - オペランド不足→`Error: not enough operands`
  - 余剰オペランド→`Error: too many operands`
  - 0除算→`Error: division by zero`
  - トークン>=10 はエラー（入力は1桁のみ）。OK
- コンテナ要件: スタックとして `std::stack` を用い、基盤コンテナは `std::list` を指定。OK
  - 具体実装:
    - `ex01/includes/ReversePolishCalculator.hpp`:
      `typedef std::stack<double, std::list<double> > DoubleStack;`
    - `ex01/class/ReversePolishCalculator.cpp`:
      計算時のスタックを `DoubleStack` に変更

---

## Exercise 02: PmergeMe
- バイナリ名: `PmergeMe`。OK
- 入力: 正の整数列、負数は `Error`。OK
- アルゴリズム: Ford-Johnson（Merge-Insert Sort）を各コンテナに対して個別実装。OK
- 出力: Before/After と 2コンテナの要素数・処理時間を表示。OK
- 時間計測: 比較回数カウントと処理時間の表示で差異が把握可能。OK
- コンテナ: `std::vector` と `std::deque` の2種を使用（ex00/ex01と重複なし）。OK

---

## 今回の修正内容
1. ex01 で指定のスタック型を導入
   - `typedef std::stack<double, std::list<double> > DoubleStack;` を追加
   - 既存の `std::stack<double>` を `DoubleStack` に置換
2. ex02 で汎用テンプレート関数を廃止し、コンテナ別の個別実装に変更
   - `vector` 用: `find_insert_pos_vec` / `generate_next_level_groups_vec` / `binary_insertion_with_jacobsthal_vec` / `fj_sort_vec`
   - `deque` 用:  `find_insert_pos_deq` / `generate_next_level_groups_deq` / `binary_insertion_with_jacobsthal_deq` / `fj_sort_deq`

以上より、チェックリストの観点（特にうるう年対応と `typedef std::stack<double, std::list<double> >` の使用）を満たしています。
