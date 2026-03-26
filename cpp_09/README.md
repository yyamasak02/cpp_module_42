# CPP MODULE 09

## ✅ Usage

```sh
cd cpp_09/exXX
make
./run_file
```

- テストケースを変えたいときは `main.cpp` を適宜更新してください。

## 📘 解説

### 🔹 概要
各課題では **コンテナクラスを1つ以上使って** 問題を解きます。  
**一度使ったコンテナクラスは他の課題で使えません。**

使用可能な C++98 コンテナクラス一覧：

#### 配列系（順序あり）:
- `std::vector`
- `std::deque`
- `std::list`

#### 連想系（キー → 値）:
- `std::map`
- `std::multimap`
- `std::set`
- `std::multiset`

⚠️ コンパイルフラグは以下を使用してください：
```sh
-Wall -Wextra -Werror -std=c++98
```

---

### 📂 ex00
ビットコインの計算をする問題。
引数にテキストファイルが与えられる。
そのファイルには日付と値が入っている、また別に日付とその日のレートを持つファイルが与えられるのでその情報をもとに値をレート変換して出力する問題

### 📂 ex01
逆ポーランド記法の計算処理を行うプログラムの作成
引数に計算式が与えられ、その結果を出力する

### 📂 ex02
数字をマージソートを利用してソートする問題
出力には、ソート前、後と２つのコンテナクラスを使ってソートするがそれぞれのコンテナの場合のソートにかかった時間を出力する。これが確か、やばい課題らしい。ベンチマークがありそう。

## 参考資料
- [The art of Programming](https://seriouscomputerist.atariverse.com/media/pdf/book/Art%20of%20Computer%20Programming%20-%20Volume%203%20(Sorting%20&%20Searching).pdf)
- [merge-insert: qiita](https://qiita.com/jizots/items/0d484a173c1921d2083e)