#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include exceptions.hpp

#include <cstddef>
#include <utility>
#include <vector>

namespace sjtu {

template <class T> class deque {
private:
  static constexpr size_t BLOCK_CAP = 128;
  struct Block {
    std::vector<T> data;
    Block() { data.reserve(BLOCK_CAP); }
  };
  std::vector<Block*> blocks;
  size_t n = 0;

  void ensure_front_block() {
    if (blocks.empty()) {
      blocks.insert(blocks.begin(), new Block());
    } else if (blocks.front()->data.size() >= BLOCK_CAP) {
      blocks.insert(blocks.begin(), new Block());
    }
  }
  void ensure_back_block() {
    if (blocks.empty() || blocks.back()->data.size() >= BLOCK_CAP) {
      blocks.push_back(new Block());
    }
  }
  void cleanup_empty_edges() {
    while (!blocks.empty() && blocks.front()->data.empty() && blocks.size() > 1) {
      delete blocks.front();
      blocks.erase(blocks.begin());
    }
    while (!blocks.empty() && blocks.back()->data.empty() && blocks.size() > 1) {
      delete blocks.back();
      blocks.pop_back();
    }
    if (blocks.empty()) blocks.push_back(new Block());
  }

  std::pair<size_t,size_t> locate(size_t pos) const {
    size_t idx = pos;
    for (size_t bi = 0; bi < blocks.size(); ++bi) {
      const auto &b = blocks[bi]->data;
      if (idx < b.size()) return {bi, idx};
      idx -= b.size();
    }
    throw index_out_of_bound();
  }

public:
  class const_iterator;
  class iterator {
    friend class deque;
  private:
    const deque *parent = nullptr;
    long long global_index = 0;
    iterator(const deque *p, long long gi): parent(p), global_index(gi) {}
  public:
    iterator() = default;
    iterator operator+(const int &k) const { return iterator(parent, global_index + k); }
    iterator operator-(const int &k) const { return iterator(parent, global_index - k); }
    int operator-(const iterator &rhs) const {
      if (parent != rhs.parent) throw invalid_iterator();
      return static_cast<int>(global_index - rhs.global_index);
    }
    iterator &operator+=(const int &k) { global_index += k; return *this; }
    iterator &operator-=(const int &k) { global_index -= k; return *this; }
    iterator operator++(int) { iterator tmp=*this; ++global_index; return tmp; }
    iterator &operator++() { ++global_index; return *this; }
    iterator operator--(int) { iterator tmp=*this; --global_index; return tmp; }
    iterator &operator--() { --global_index; return *this; }
    T &operator*() const { return const_cast<deque*>(parent)->at(static_cast<size_t>(global_index)); }
    T *operator->() const noexcept { return &const_cast<deque*>(parent)->at(static_cast<size_t>(global_index)); }
    bool operator==(const iterator &rhs) const { return parent==rhs.parent && global_index==rhs.global_index; }
    bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
    bool operator==(const const_iterator &rhs) const;
    bool operator!=(const const_iterator &rhs) const;
  };

  class const_iterator {
    friend class deque;
  private:
    const deque *parent = nullptr;
    long long global_index = 0;
    const_iterator(const deque *p, long long gi): parent(p), global_index(gi) {}
  public:
    const_iterator() = default;
    const_iterator(const iterator &it): parent(it.parent), global_index(it.global_index) {}
    const_iterator operator+(const int &k) const { return const_iterator(parent, global_index + k); }
    const_iterator operator-(const int &k) const { return const_iterator(parent, global_index - k); }
    int operator-(const const_iterator &rhs) const {
      if (parent != rhs.parent) throw invalid_iterator();
      return static_cast<int>(global_index - rhs.global_index);
    }
    const_iterator &operator+=(const int &k) { global_index += k; return *this; }
    const_iterator &operator-=(const int &k) { global_index -= k; return *this; }
    const_iterator operator++(int) { const_iterator tmp=*this; ++global_index; return tmp; }
    const_iterator &operator++() { ++global_index; return *this; }
    const_iterator operator--(int) { const_iterator tmp=*this; --global_index; return tmp; }
    const_iterator &operator--() { --global_index; return *this; }
    const T &operator*() const { return parent->at(static_cast<size_t>(global_index)); }
    const T *operator->() const noexcept { return &parent->at(static_cast<size_t>(global_index)); }
    bool operator==(const const_iterator &rhs) const { return parent==rhs.parent && global_index==rhs.global_index; }
    bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
    bool operator==(const iterator &rhs) const { return parent==rhs.parent && global_index==rhs.global_index; }
    bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
  };

  bool iterator::operator==(const const_iterator &rhs) const { return parent==rhs.parent && global_index==rhs.global_index; }
  bool iterator::operator!=(const const_iterator &rhs) const { return !(*this==rhs); }

  deque() { blocks.push_back(new Block()); }
  deque(const deque &other) : n(other.n) {
    for (auto *b : other.blocks) {
      Block *nb = new Block();
      nb->data = b->data;
      blocks.push_back(nb);
    }
    if (blocks.empty()) blocks.push_back(new Block());
  }
  ~deque() { clear(); for (auto *b : blocks) delete b; blocks.clear(); }

  deque &operator=(const deque &other) {
    if (this == &other) return *this;
    clear();
    for (auto *b : blocks) delete b;
    blocks.clear();
    n = other.n;
    for (auto *b : other.blocks) {
      Block *nb = new Block();
      nb->data = b->data;
      blocks.push_back(nb);
    }
    if (blocks.empty()) blocks.push_back(new Block());
    return *this;
  }

  T &at(const size_t &pos) {
    if (pos >= n) throw index_out_of_bound();
    auto [bi, off] = locate(pos);
    return blocks[bi]->data[off];
  }
  const T &at(const size_t &pos) const {
    if (pos >= n) throw index_out_of_bound();
    auto [bi, off] = locate(pos);
    return blocks[bi]->data[off];
  }
  T &operator[](const size_t &pos) { return at(pos); }
  const T &operator[](const size_t &pos) const { return at(pos); }

  const T &front() const { if (n==0) throw container_is_empty(); return blocks.front()->data.front(); }
  const T &back() const { if (n==0) throw container_is_empty(); return blocks.back()->data.back(); }

  iterator begin() { return iterator(this, 0); }
  const_iterator cbegin() const { return const_iterator(this, 0); }
  iterator end() { return iterator(this, static_cast<long long>(n)); }
  const_iterator cend() const { return const_iterator(this, static_cast<long long>(n)); }

  bool empty() const { return n==0; }
  size_t size() const { return n; }

  void clear() {
    for (auto *b : blocks) { b->data.clear(); }
    n = 0;
    cleanup_empty_edges();
  }

  iterator insert(iterator pos, const T &value) {
    if (pos.parent != this) throw invalid_iterator();
    size_t gi = static_cast<size_t>(pos.global_index);
    if (gi > n) throw index_out_of_bound();
    if (gi == n) { push_back(value); return iterator(this, static_cast<long long>(n-1)); }
    auto [bi, off] = locate(gi);
    auto &vec = blocks[bi]->data;
    if (vec.size() >= BLOCK_CAP) {
      Block *nb = new Block();
      size_t move = vec.size()/2;
      nb->data.insert(nb->data.end(), vec.begin()+move, vec.end());
      vec.erase(vec.begin()+move, vec.end());
      blocks.insert(blocks.begin()+bi+1, nb);
      if (off >= vec.size()) { bi += 1; off -= move; }
    }
    vec.insert(vec.begin()+off, value);
    ++n;
    return iterator(this, pos.global_index);
  }

  iterator erase(iterator pos) {
    if (empty()) throw container_is_empty();
    if (pos.parent != this) throw invalid_iterator();
    size_t gi = static_cast<size_t>(pos.global_index);
    if (gi >= n) throw index_out_of_bound();
    auto [bi, off] = locate(gi);
    auto &vec = blocks[bi]->data;
    vec.erase(vec.begin()+off);
    --n;
    if (bi+1 < blocks.size() && vec.size()+blocks[bi+1]->data.size() < BLOCK_CAP/2) {
      vec.insert(vec.end(), blocks[bi+1]->data.begin(), blocks[bi+1]->data.end());
      delete blocks[bi+1];
      blocks.erase(blocks.begin()+bi+1);
    }
    cleanup_empty_edges();
    if (gi >= n) return end();
    return iterator(this, static_cast<long long>(gi));
  }

  void push_back(const T &value) {
    ensure_back_block();
    blocks.back()->data.push_back(value);
    ++n;
  }

  void pop_back() {
    if (empty()) throw container_is_empty();
    blocks.back()->data.pop_back();
    --n;
    cleanup_empty_edges();
  }

  void push_front(const T &value) {
    ensure_front_block();
    blocks.front()->data.insert(blocks.front()->data.begin(), value);
    ++n;
  }

  void pop_front() {
    if (empty()) throw container_is_empty();
    auto &vec = blocks.front()->data;
    vec.erase(vec.begin());
    --n;
    cleanup_empty_edges();
  }
};

} // namespace sjtu

#endif

