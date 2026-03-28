#include "../includes/PmergeMe.hpp"
#include <climits>
#include <ctime>
#include <sstream>

/**
 * 比較回数カウンタを初期化する
 */
PmergeMe::PmergeMe() : compare_count_(0) {}

/**
 * 特別な後始末は不要
 */
PmergeMe::~PmergeMe() {}

/**
 * 比較回数カウンタをコピーする
 * @param copy コピー元
 */
PmergeMe::PmergeMe(const PmergeMe &copy) : compare_count_(copy.compare_count_) {}

/**
 * 自己代入を避けつつ比較回数カウンタをコピーする
 * @param copy コピー元
 * @return 自身への参照
 */
PmergeMe &PmergeMe::operator=(const PmergeMe &copy)
{
    if (this != &copy)
    {
        this->compare_count_ = copy.compare_count_;
    }
    return *this;
}

/**
 * 文字列を正の整数に変換する
 * 変換失敗時は -1 を返す（負数/非数/オーバーフローを含む）
 * @param str 入力文字列
 * @return 変換結果（失敗時は -1）
 */
int PmergeMe::convert_positive_int(const std::string &str)
{
    std::stringstream ss(str);
    long value;
    ss >> value;
    if (ss.fail() || !ss.eof() || value <= 0 || value > INT_MAX)
        return -1;
    return static_cast<int>(value);
}

/**
 * 文字列配列から正の整数配列を生成する（無効入力で例外）
 * @param str_ptr 文字列配列の先頭ポインタ
 * @param size 要素数
 * @return 新規に確保された整数配列（呼び出し側で delete[] が必要）
 * @exception PmergeMe::InvalidInput 無効な入力を検出した場合
 */
int *PmergeMe::create_numbers(char **str_ptr, int size)
{
    int *array = new int[size];
    for (int i = 0; i < size; i++)
    {
        int res = PmergeMe::convert_positive_int(std::string(str_ptr[i]));
        if (res == -1)
        {
            delete[] array;
            throw PmergeMe::InvalidInput();
        }
        array[i] = res;
    }
    return array;
}

/**
 * ヤコブスタール数 J(n) に基づくバッチ挿入サイズ（J(n)*2）を返す
 * J(n) = (2^n - (-1)^n) / 3
 * @param n 段数
 * @return バッチ挿入サイズ（J(n)*2）
 */
size_t PmergeMe::jacobsthal_number(size_t n)
{
    int sign = (n % 2 == 0) ? -1 : 1;
    unsigned long long pow2 = 1ULL;
    for (size_t i = 0; i < n; i++)
        pow2 *= 2ULL;
    long long res = static_cast<long long>(pow2) + sign;
    res = (res / 3) * 2;
    return static_cast<size_t>(res);
}

/**
 * vector で Ford-Johnson ソートを実行し、結果/時間/比較回数を表示
 * @param vec 単一要素チェインの列
 * @return なし
 */
void PmergeMe::sort_vec(ChainVecList &vec)
{
    std::clock_t start = std::clock();
    ChainVecList sorted_container = fj_sort(vec);
    std::clock_t end = std::clock();

    std::cout << "After:  ";
    for (size_t i = 0; i < sorted_container.size(); i++)
        std::cout << sorted_container[i][0] << " ";
    std::cout << std::endl;

    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;
    std::cout << "Time to process a range of " << std::setw(4) << std::setfill(' ') << sorted_container.size()
              << " elements with std::vector : " << elapsed << " us" << std::endl;
    #ifdef DEBUG
        std::cout << "Number of comparisons with std::vector : " << this->compare_count_ << std::endl;
    #endif
}

/**
 * deque で Ford-Johnson ソートを実行し、時間/比較回数を表示
 * @param deq 単一要素チェインの列
 * @return なし
 */
void PmergeMe::sort_deq(ChainDeqList &deq)
{
    std::clock_t start = std::clock();
    ChainDeqList sorted_container = fj_sort(deq);
    std::clock_t end = std::clock();

    double elapsed = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;
    std::cout << "Time to process a range of " << std::setw(4) << std::setfill(' ') << sorted_container.size()
              << " elements with std::deque  : " << elapsed << " us" << std::endl;
    #ifdef DEBUG
        std::cout << "Number of comparisons with std::deque  : " << this->compare_count_ << std::endl;
    #endif
}

/**
 * 単一要素チェイン列を構築し、vector と deque の両方でソート実行
 * @param array 入力整数配列
 * @param size 要素数
 * @return なし
 */
void PmergeMe::execute_sort(const int *array, int size)
{
    std::cout << "Before: ";
    ChainVecList vec = ChainVecList();
    ChainDeqList deq = ChainDeqList();

    for (int i = 0; i < size; i++)
    {
        std::cout << array[i] << " ";
        vec.push_back(ChainVec(1, array[i]));
        deq.push_back(ChainDeq(1, array[i]));
    }
    std::cout << std::endl;

    resetCompareCount();
    sort_vec(vec);
    resetCompareCount();
    sort_deq(deq);
}

/**
 * 無効な入力例外のメッセージ
 * @return メッセージの C 文字列
 */
const char *PmergeMe::InvalidInput::what() const throw()
{
    return "Invalid input";
}

/**
 * 比較回数カウンタを 0 にリセットする
 * @return 自身へのポインタ
 */
PmergeMe *PmergeMe::resetCompareCount()
{
    this->compare_count_ = 0;
    return this;
}

/**
 * 二分探索で、代表値（back）に基づき挿入位置を求める（vector）
 * @param first 探索範囲の先頭イテレータ
 * @param last 探索範囲の終端イテレータ
 * @param val 比較に用いる代表値
 * @return 挿入位置のイテレータ
 */
PmergeMe::ChainVecList::iterator PmergeMe::find_insert_pos(ChainVecList::iterator first, ChainVecList::iterator last,
                                                           int val)
{
    while (first < last)
    {
        ChainVecList::iterator mid = first + (last - first) / 2;
        this->compare_count_++;
        if (mid->back() < val)
            first = mid + 1;
        else
            last = mid;
    }
    return first;
}

/**
 * 隣接ペアを代表値で比較し、小→大の順に結合して次レベルを生成（vector）
 * @param groups 現在のグループ列
 * @return 次レベルのグループ列
 */
PmergeMe::ChainVecList PmergeMe::generate_next_level_groups(const ChainVecList &groups)
{
    ChainVecList pairs;
    for (size_t i = 0; i + 1 < groups.size(); i += 2)
    {
        const ChainVec &first = groups[i];
        const ChainVec &second = groups[i + 1];

        this->compare_count_++;
        bool second_is_smaller = (second.back() < first.back());
        const ChainVec &smaller = second_is_smaller ? second : first;
        const ChainVec &bigger = second_is_smaller ? first : second;

        ChainVec merged;
        merged.insert(merged.end(), smaller.begin(), smaller.end());
        merged.insert(merged.end(), bigger.begin(), bigger.end());
        pairs.push_back(merged);
    }
    return pairs;
}

/**
 * Jacobsthal バッチ挿入で、大半分→余り→小半分（逆順）を二分挿入（vector）
 * @param records 再帰で得た基準レコード列（大半分）
 * @param extra_elem 余り要素（奇数個時）
 * @param has_extra 余り要素の有無
 * @return 挿入後のグループ列
 */
PmergeMe::ChainVecList PmergeMe::binary_insertion_with_jacobsthal(const ChainVecList &records,
                                                                  const ChainVec &extra_elem, const bool has_extra)
{
    ChainVecList result;
    size_t batch_start = 0;
    size_t batch_end = 1;
    int batch_idx = 1;
    bool is_done = false;

    while (!is_done)
    {
        if (batch_end >= records.size())
        {
            batch_end = records.size();
            is_done = true;
        }

        // 大半分を追加
        for (size_t p = batch_start; p < batch_end; p++)
        {
            size_t half_size = records[p].size() / 2;
            ChainVec large(records[p].begin() + half_size, records[p].end());
            result.push_back(large);
        }

        // 余り要素を挿入
        if (is_done && has_extra)
        {
            result.insert(find_insert_pos(result.begin(), result.end(), extra_elem.back()), extra_elem);
        }

        // 小半分を逆順で挿入
        ChainVec inserted_keys;
        for (size_t p = batch_end; p > batch_start; p--)
        {
            size_t half_size = records[p - 1].size() / 2;
            size_t search_end = (is_done && has_extra) ? batch_start + p : batch_start + p - 1;

            for (ChainVec::iterator it = inserted_keys.begin(); it != inserted_keys.end(); ++it)
            {
                if (records[p - 1].back() >= *it)
                    search_end++;
            }

            int key = records[p - 1][half_size - 1];
            ChainVec small(records[p - 1].begin(), records[p - 1].begin() + half_size);
            inserted_keys.push_back(key);
            result.insert(find_insert_pos(result.begin(), result.begin() + search_end, key), small);
        }

        batch_start = batch_end;
        batch_end = batch_start + jacobsthal_number(batch_idx);
        batch_idx++;
    }
    return result;
}

/**
 * 再帰的にグループ化し Jacobsthal 挿入で整列（vector）
 * @param groups 入力のグループ列
 * @return 整列済みのグループ列
 */
PmergeMe::ChainVecList PmergeMe::fj_sort(const ChainVecList &groups)
{
    if (groups.size() == 1)
        return groups;

    ChainVecList rec = fj_sort(this->generate_next_level_groups(groups));
    return this->binary_insertion_with_jacobsthal(rec, groups.back(), static_cast<bool>(groups.size() % 2));
}

/**
 * 二分探索で、代表値（back）に基づき挿入位置を求める（deque）
 * @param first 探索範囲の先頭イテレータ
 * @param last 探索範囲の終端イテレータ
 * @param val 比較に用いる代表値
 * @return 挿入位置のイテレータ
 */
PmergeMe::ChainDeqList::iterator PmergeMe::find_insert_pos(ChainDeqList::iterator first, ChainDeqList::iterator last,
                                                           int val)
{
    while (first < last)
    {
        ChainDeqList::iterator mid = first + (last - first) / 2;
        this->compare_count_++;
        if (mid->back() < val)
            first = mid + 1;
        else
            last = mid;
    }
    return first;
}

/**
 * 隣接ペアを代表値で比較し、小→大の順に結合して次レベルを生成（deque）
 * @param groups 現在のグループ列
 * @return 次レベルのグループ列
 */
PmergeMe::ChainDeqList PmergeMe::generate_next_level_groups(const ChainDeqList &groups)
{
    ChainDeqList pairs;
    for (size_t i = 0; i + 1 < groups.size(); i += 2)
    {
        const ChainDeq &first = groups[i];
        const ChainDeq &second = groups[i + 1];

        this->compare_count_++;
        bool second_is_smaller = (second.back() < first.back());
        const ChainDeq &smaller = second_is_smaller ? second : first;
        const ChainDeq &bigger = second_is_smaller ? first : second;

        ChainDeq merged;
        merged.insert(merged.end(), smaller.begin(), smaller.end());
        merged.insert(merged.end(), bigger.begin(), bigger.end());
        pairs.push_back(merged);
    }
    return pairs;
}

/**
 * Jacobsthal バッチ挿入で、大半分→余り→小半分（逆順）を二分挿入（deque）
 * @param records 再帰で得た基準レコード列（大半分）
 * @param extra_elem 余り要素（奇数個時）
 * @param has_extra 余り要素の有無
 * @return 挿入後のグループ列
 */
PmergeMe::ChainDeqList PmergeMe::binary_insertion_with_jacobsthal(const ChainDeqList &records,
                                                                  const ChainDeq &extra_elem, const bool has_extra)
{
    ChainDeqList result;
    size_t batch_start = 0;
    size_t batch_end = 1;
    int batch_idx = 1;
    bool is_done = false;

    while (!is_done)
    {
        if (batch_end >= records.size())
        {
            batch_end = records.size();
            is_done = true;
        }

        for (size_t p = batch_start; p < batch_end; p++)
        {
            size_t half_size = records[p].size() / 2;
            ChainDeq large(records[p].begin() + half_size, records[p].end());
            result.push_back(large);
        }

        if (is_done && has_extra)
        {
            result.insert(find_insert_pos(result.begin(), result.end(), extra_elem.back()), extra_elem);
        }

        ChainDeq inserted_keys;
        for (size_t p = batch_end; p > batch_start; p--)
        {
            size_t half_size = records[p - 1].size() / 2;
            size_t search_end = (is_done && has_extra) ? batch_start + p : batch_start + p - 1;

            for (ChainDeq::iterator it = inserted_keys.begin(); it != inserted_keys.end(); ++it)
            {
                if (records[p - 1].back() >= *it)
                    search_end++;
            }

            int key = records[p - 1][half_size - 1];
            ChainDeq small(records[p - 1].begin(), records[p - 1].begin() + half_size);
            inserted_keys.push_back(key);
            result.insert(find_insert_pos(result.begin(), result.begin() + search_end, key), small);
        }

        batch_start = batch_end;
        batch_end = batch_start + jacobsthal_number(batch_idx);
        batch_idx++;
    }
    return result;
}

/**
 * 再帰的にグループ化し Jacobsthal 挿入で整列（deque）
 * @param groups 入力のグループ列
 * @return 整列済みのグループ列
 */
PmergeMe::ChainDeqList PmergeMe::fj_sort(const ChainDeqList &groups)
{
    if (groups.size() == 1)
        return groups;

    ChainDeqList rec = fj_sort(this->generate_next_level_groups(groups));
    return this->binary_insertion_with_jacobsthal(rec, groups.back(), static_cast<bool>(groups.size() % 2));
}
