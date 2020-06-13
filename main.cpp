#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#if __GNUC__ >= 10
#include <ranges>
#endif

using namespace std;

int inc(int x) {
  return x + 1;
}

int fib(int n) {
  switch (n) {
    case 0:
    case 1:
      return n;
    default:
      return fib(n - 2) + fib(n - 1);
  }
}

auto fact(int x) {
  if (x == 0)
    return 1;
  else {
    auto tmp = fact(x - 1);
    return tmp % 100 * x;
  }
}

template <launch policy>
auto makeLazyVec = [](auto vars) {
  vector<shared_future<int>> fvars;
  transform(begin(vars), end(vars), back_inserter(fvars),
            [](int x) -> shared_future<int> {
              return async(
                         policy,
                         [](int y) -> int { return y; },
                         x)
                  .share();
            });
  return fvars;
};

auto lazyMap = [](auto in, auto func) -> decltype(in) {
  decltype(in) nxt;
  transform(begin(in), end(in), back_inserter(nxt), func);
  return nxt;
};

template <launch policy>
auto makeLazyFunc = [](auto func) {
  return [func](auto in) {
    return async(
        policy, [func](auto in) { return func(in.get()); }, in);
  };
};

auto lazyTake = [](auto vec, int num) {
  decltype(vec) ret;
  copy_n(begin(vec), min((int)vec.size(), num), back_inserter(ret));
  return ret;
};

class stopWatch {
 private:
  decltype(std::chrono::high_resolution_clock::now()) start = std::chrono::high_resolution_clock::now();

 public:
  ~stopWatch() {
    const auto finish = std::chrono::high_resolution_clock::now();
    const auto durationMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    std::cout << durationMilliSeconds << " ms" << std::endl;
  }
};

template <launch policy>
class lazyVec {
 private:
  const vector<shared_future<int>> _fv;

 public:
  lazyVec(const vector<int>& v) : _fv{makeLazyVec<policy>(v)} {}
  lazyVec(const vector<shared_future<int>>& v) : _fv{v} {}

  decltype(_fv.cbegin()) begin() const noexcept {
    return _fv.cbegin();
  }
  decltype(_fv.cend()) end() const noexcept {
    return _fv.cend();
  }

  template <typename TFunc>
  const lazyVec map(TFunc func) const {
    return lazyVec(lazyMap(_fv, func));
  }

  const lazyVec take(int num) const {
    return lazyTake(_fv, num);
  }

  long long accumulate() const {
    return std::accumulate(_fv.begin(), _fv.end(), 0LL, [](auto l, auto r) { return l + r.get(); });
  }
};

constexpr int VARSNUM = 40;
constexpr int TAKENUM = 40;
static_assert(VARSNUM >= TAKENUM);

int main() {
  vector<int> vars;
  for (auto i = 0; i < VARSNUM; i++) vars.push_back(i);
  {  // deferred
    stopWatch t;
    vector<shared_future<int>> fvars = makeLazyVec<launch::deferred>(vars);
    fvars = lazyMap(fvars, makeLazyFunc<launch::deferred>(fib));
    fvars = lazyMap(fvars, makeLazyFunc<launch::deferred>(inc));
    fvars = lazyMap(fvars, makeLazyFunc<launch::deferred>(inc));
    fvars = lazyTake(fvars, TAKENUM);
    cout << accumulate(fvars.begin(), fvars.end(), 0LL, [](auto l, auto r) { return l + r.get(); }) << endl;
  }
  {  // method chain, deferred
    stopWatch t;
    const auto v = lazyVec<launch::deferred>(vars);
    const auto ans = v.map(makeLazyFunc<launch::deferred>(fib))
                         .map(makeLazyFunc<launch::deferred>(inc))
                         .map(makeLazyFunc<launch::deferred>(inc))
                         .take(TAKENUM)
                         .accumulate();
    cout << ans << endl;
  }
  {  // async
    stopWatch t;
    const auto v = lazyVec<launch::async>(vars);
    const auto ans = v.map(makeLazyFunc<launch::async>(fib))
                         .map(makeLazyFunc<launch::async>(inc))
                         .map(makeLazyFunc<launch::async>(inc))
                         .take(TAKENUM)
                         .accumulate();
    cout << ans << endl;
  }
#if __GNUC__ >= 10
  {  // ranges
    stopWatch t;
    long long base = 0;
    for (auto i : vars | views::transform(fib) | views::transform(inc) | views::transform(inc) | views::take(TAKENUM)) base += i;

    cout << base << endl;
  }
#endif
}
