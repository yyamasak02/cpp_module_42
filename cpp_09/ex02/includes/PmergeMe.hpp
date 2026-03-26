#pragma once

#include <deque>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class PmergeMe
{
  public:
    PmergeMe();
    ~PmergeMe();
    static int convert_positive_int(const std::string &str);
    static int *create_numbers(char **str_ptr, int size);
    void execute_sort(const int *array, int size);

    class PmergeMeException : public std::exception
    {
      public:
        virtual const char *what() const throw() = 0;
    };
    class InvalidInput : public PmergeMeException
    {
      public:
        virtual const char *what() const throw();
    };

  private:
    PmergeMe(const PmergeMe &copy);
    PmergeMe &operator=(const PmergeMe &copy);

    typedef std::vector<int> ChainVec;
    typedef std::vector<ChainVec> ChainVecList;
    typedef std::deque<int> ChainDeq;
    typedef std::deque<ChainDeq> ChainDeqList;

    int compare_count_;

    static size_t jacobsthal_number(size_t n);

    template <typename Iter> Iter find_insert_pos(Iter first, Iter last, int val)
    {
        while (first < last)
        {
            Iter mid = first + (last - first) / 2;
            this->compare_count_++;
            if (mid->back() < val)
                first = mid + 1;
            else
                last = mid;
        }
        return first;
    }

    /**
     * 代表値（back）で比較し、次の階層のグループ（ペア）を作成する
     * CV: 外側のコンテナ型 (ChainVec or ChainDeqVec)
     */
    template <typename CV> CV generate_next_level_groups(const CV &groups)
    {
        // CV の要素型（Chain または ChainDeq）を明示的にエイリアスとして定義
        typedef typename CV::value_type C;

        CV pairs;
        for (size_t i = 0; i + 1 < groups.size(); i += 2)
        {
            const C &first = groups[i];
            const C &second = groups[i + 1];

            this->compare_count_++;

            // 代表値（末尾要素）を比較
            bool second_is_smaller = (second.back() < first.back());
            const C &smaller = second_is_smaller ? second : first;
            const C &bigger = second_is_smaller ? first : second;

            C merged;
            // 小さいグループを先に、大きいグループを後に結合
            merged.insert(merged.end(), smaller.begin(), smaller.end());
            merged.insert(merged.end(), bigger.begin(), bigger.end());

            pairs.push_back(merged);
        }
        return pairs;
    }

    template <typename CV>
    CV binary_insertion_with_jacobsthal(const CV &records, const typename CV::value_type &extra_elem,
                                        const bool has_extra)
    {
        typedef typename CV::value_type C;

        CV result;
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

            // 大半分の追加
            for (size_t p = batch_start; p < batch_end; p++)
            {
                size_t half_size = records[p].size() / 2;
                C large(records[p].begin() + half_size, records[p].end());
                result.push_back(large);
            }

            // 余り要素の挿入
            if (is_done && has_extra)
            {
                result.insert(find_insert_pos(result.begin(), result.end(), extra_elem.back()), extra_elem);
            }

            // 小半分の逆順挿入
            C inserted_keys;
            for (size_t p = batch_end; p > batch_start; p--)
            {
                size_t half_size = records[p - 1].size() / 2;
                size_t search_end = (is_done && has_extra) ? batch_start + p : batch_start + p - 1;

                for (typename C::iterator it = inserted_keys.begin(); it != inserted_keys.end(); ++it)
                {
                    if (records[p - 1].back() >= *it)
                        search_end++;
                }

                int key = records[p - 1][half_size - 1];
                C small(records[p - 1].begin(), records[p - 1].begin() + half_size);
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
     * Ford-Johnson ソートの再帰処理（deque版）
     */
    template <typename CV> CV fj_sort(const CV &groups)
    {
        if (groups.size() == 1)
            return groups;

        CV rec = fj_sort(this->generate_next_level_groups(groups));
        return this->binary_insertion_with_jacobsthal(rec, groups.back(), static_cast<bool>(groups.size() % 2));
    }

    void sort_vec(ChainVecList &vec);
    void sort_deq(ChainDeqList &deq);
    PmergeMe *resetCompareCount();
};
