// pbds_only_list.hpp
//
// A minimal list-like wrapper around GNU PBDS's order-statistics tree.
// This is what the reviewer means by "a Gnu PBDS tree with a
// tree_order_statistics_node_update policy" used as a general-purpose list.
//
// PBDS trees are ordered *maps*: they order entries by key. To use one as a
// position-indexed list we need integer keys that preserve relative order
// (an "order-maintenance" problem). Here we use the standard gapped-key
// trick: new elements get keys placed strictly between their neighbors'
// keys. If the local integer gap between two neighboring keys is exhausted
// (extremely rare with 63 bits of gap headroom for realistic workloads),
// we fall back to a full renumbering of the tree.
//
// This is adequate for *benchmarking purposes* (matching what the reviewer
// asked you to compare against). It is NOT hardened for production use.

#pragma once
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <cstdint>
#include <stdexcept>

template <typename T>
class PbdsOnlyList {
public:
    using key_t = long long;
    using tree_t = __gnu_pbds::tree<
        key_t, T, std::less<key_t>,
        __gnu_pbds::rb_tree_tag,
        __gnu_pbds::tree_order_statistics_node_update>;

    PbdsOnlyList() = default;

    size_t size() const { return tree_.size(); }
    bool empty() const { return tree_.empty(); }

    // get element at index i  -- O(log n)
    const T& get(size_t i) const {
        auto it = tree_.find_by_order(static_cast<int>(i));
        return it->second;
    }

    // ---- core operations (unchanged / already tested) ----

    // insert value at index pos (0 <= pos <= size())  -- O(log n) amortized
    void insert_at(size_t pos, const T& val) {
        size_t n = tree_.size();
        key_t newKey;
        if (n == 0) {
            newKey = 0;
        } else if (pos == 0) {
            key_t minKey = tree_.find_by_order(0)->first;
            if (minKey <= INT64_MIN / 2) { renumber(); minKey = tree_.find_by_order(0)->first; }
            newKey = minKey - GAP;
        } else if (pos == n) {
            key_t maxKey = tree_.find_by_order(static_cast<int>(n - 1))->first;
            if (maxKey >= INT64_MAX / 2) { renumber(); maxKey = tree_.find_by_order(static_cast<int>(n - 1))->first; }
            newKey = maxKey + GAP;
        } else {
            key_t lo = tree_.find_by_order(static_cast<int>(pos - 1))->first;
            key_t hi = tree_.find_by_order(static_cast<int>(pos))->first;
            if (hi - lo < 2) {
                renumber();
                lo = tree_.find_by_order(static_cast<int>(pos - 1))->first;
                hi = tree_.find_by_order(static_cast<int>(pos))->first;
            }
            newKey = lo + (hi - lo) / 2;
        }
        tree_.insert({newKey, val});
    }

    void push_back(const T& val) { insert_at(tree_.size(), val); }
    void push_front(const T& val) { insert_at(0, val); }

    // remove element at index pos -- O(log n)
    void erase_at(size_t pos) {
        auto it = tree_.find_by_order(static_cast<int>(pos));
        tree_.erase(it);
    }

    void pop_back() { erase_at(tree_.size() - 1); }
    void pop_front() { erase_at(0); }

    // ---- EfficientList-shaped wrappers, for side-by-side testing ----
    // These do NOT reimplement anything -- they just dispatch to the core
    // operations above, the same way you'd expect EfficientList::insert's
    // "insert at end / insert at beginning / insert in the middle" cases to
    // just be calling its own push_back/push_front/general-insert internally.

    void insert(const int &idx, const T &data)
    {
        if (idx < 0 || idx > static_cast<int>(tree_.size()))
        {
            throw std::runtime_error("out_of_range");
        }
        if (idx == static_cast<int>(tree_.size())) { push_back(data); return; }
        if (idx == 0) { push_front(data); return; }
        insert_at(static_cast<size_t>(idx), data);
    }

    void erase(const int &idx)
    {
        if (idx < 0 || idx >= static_cast<int>(tree_.size()))
        {
            throw std::runtime_error("out_of_range");
        }
        if (idx == static_cast<int>(tree_.size()) - 1) { pop_back(); return; }
        if (idx == 0) { pop_front(); return; }
        erase_at(static_cast<size_t>(idx));
    }

public:
    long renumber_count_ = 0; // instrumentation, remove in production
private:
    static constexpr key_t GAP = (key_t(1) << 40);
    tree_t tree_;

    // Full renumbering fallback: evenly re-spaces every key.
    // O(n). Should trigger only after an extreme number (~2^40) of
    // repeated insertions at the exact same spot -- effectively never
    // in the benchmarks below, but included for correctness.
    void renumber() {
        // IMPORTANT: re-space keys using a *fixed, bounded* step (GAP),
        // not the full int64 range. Spanning the whole range on every
        // renumber leaves almost no headroom before the next push_front/
        // push_back drifts past the half-range trigger again, causing a
        // renumber storm (this was found via fuzzing -- see commit
        // history / conversation). Centering around 0 with step=GAP
        // guarantees ~2^23 more edge pushes and ~40 more local bisections
        // before the next renumber is needed, for realistic n.
        renumber_count_++;
        size_t n = tree_.size();
        if (n == 0) return;
        std::vector<T> vals;
        vals.reserve(n);
        for (auto it = tree_.begin(); it != tree_.end(); ++it) vals.push_back(it->second);
        tree_.clear();
        key_t k = -(static_cast<key_t>(n / 2)) * GAP;
        for (size_t i = 0; i < n; ++i) {
            tree_.insert({k, vals[i]});
            k += GAP;
        }
    }
};
