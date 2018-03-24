//~ #include <bits/stdc++.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cassert>
#include <vector>
#include <cmath>
using namespace std;
typedef long long lli;
typedef std::pair<int,int> pii;

class SegmentTree{
private:
	struct Data{
		int mn, mx, sm, lz, add;
		bool islz;
		Data(){mn = mx = sm = lz = add = islz = 0;}
		inline void fixed_down(int sz, Data& l, Data& r){
			if (islz){
				sm = sz * lz;
				mn = mx = lz;
				if (sz > 1){
					l.lz   = r.lz   = lz;
					l.add  = r.add  = 0;
					l.islz = r.islz = true;
				}
				islz = false;
			}
			sm += add * sz;
			mn += add; mx += add;
			if (sz > 1){
				l.add += add;
				r.add += add;
			}
			add = 0;
		}
		inline void fixed_up(Data& l, Data& r){
			mx = std::max(l.mx, r.mx);
			mn = std::min(l.mn, r.mn);
			sm = l.sm + r.sm;
		}
	}; // struct Data(node)
	
	// segment traversal
	void traversal(int x, int l, int r, int bl, int br, const auto& op){
		auto& lnd = sg[bl + 1 < br ? x * 2 : 0];
		auto& rnd = sg[bl + 1 < br ? x * 2 + 1 : 0];
		sg[x].fixed_down(br - bl, lnd, rnd);
		if (r <= bl or  br <= l) return;
		if (l <= bl and br <= r){
			op(sg[x]);
			sg[x].fixed_down(br - bl, lnd, rnd);
		}else{
			int m = (bl + br) >> 1;
			traversal(x * 2, l, r, bl, m, op);
			traversal(x * 2 + 1, l, r, m, br, op);
			sg[x].fixed_up(lnd, rnd);
		}
	};
	
	// class member
	int n;
	int sz;
	Data* sg;
public:
	// __init__
	SegmentTree(int _n): n(1 << int(ceil(log2(_n)))), 
		sz(_n), sg(new Data[n * 2]{}) {
		assert(std::all_of(sg, sg + n * 2, [](auto d){return !d.sm and !d.mn and !d.mx;}));
	}
	~SegmentTree(){delete[] sg;}
	
	// __getitem__
#define res_method1(nm, init, ...) \
	int nm(int l, int r){\
		int rs = init;\
		traversal(1, l, r, 0, n, [&rs](auto& d){rs = __VA_ARGS__;});\
		return rs;\
	}
	res_method1(min,  1e9, std::min(rs, d.mn));
	res_method1(max, -1e9, std::max(rs, d.mx));
	res_method1(sum,  0, rs + d.sm);
	
	// __setitem__
#define res_method2(nm, ...) \
	void nm(int l, int r, int v){\
		traversal(1, l, r, 0, n, [&v](auto& d){__VA_ARGS__;});\
	}	
	res_method2(set, d.islz = true; d.add = 0; d.lz = v;);
	res_method2(add, d.add += v);
#undef res_method1	
#undef res_method2

	// misseranious
	int  size (){return sz;}
	bool empty(){return sz == 0;}
	int operator [](int i){return sum(i, i + 1);}

	// iterator
	struct iterator {
		int idx;
		SegmentTree* sg;
		iterator(SegmentTree* s, int x): idx(x), sg(s) {}
		int operator* (){return sg->sum(idx, idx + 1);}
		iterator operator++(){++idx; return *this;}
		iterator operator--(){--idx; return *this;}
		iterator operator++(int){auto tmp = *this; ++idx; return tmp;}
		iterator operator--(int){auto tmp = *this; --idx; return tmp;}
		bool operator!= (iterator rhs){return tie(sg, idx) != tie(rhs.sg, rhs.idx);}
		bool operator== (iterator rhs){return tie(sg, idx) == tie(rhs.sg, rhs.idx);}
	};
	iterator begin(){return iterator(this, 0);}
	iterator end  (){return iterator(this, sz);}
};

struct BruteSegTree : public std::vector<int> {
	typedef std::vector<int> Base;
	using Base::Base;
	using Base::begin;
	auto add(int l, int r, int vl){std::transform(begin() + l, begin() + r, begin() + l, [&](int it){return it + vl;});}
	auto set(int l, int r, int vl){std::fill(begin() + l, begin() + r, vl);}
	auto min(int l, int r){return *std::min_element(begin() + l, begin() + r);}
	auto max(int l, int r){return *std::max_element(begin() + l, begin() + r);}
	auto sum(int l, int r){return std::accumulate(begin() + l, begin() + r, 0LL);}
};

void unit_test(int n, int t, auto rnd){
	SegmentTree  s(n);
	BruteSegTree v(n);
	int l, r, vl;
	auto gen_range = [&](){
		vl = (rand() & 0xffff) * (rand() & 1 ?: -1);
		do{
			l  = rnd() % (n + 1);
			r  = rnd() % (n + 1);
			tie(l, r) = pii(std::minmax(l, r));
		}while(l == r);
	};
	// usage example
	auto add = [&](){s.add(l, r, vl); v.add(l, r, vl);};
	auto set = [&](){s.set(l, r, vl); v.set(l, r, vl);};
	auto min = [&](){assert(s.min(l, r) == v.min(l, r));};
	auto max = [&](){assert(s.max(l, r) == v.max(l, r));};
	auto sum = [&](){assert(s.sum(l, r) == v.sum(l, r));};
	const std::vector<std::function<void(void)>> func = {add, set, min, max, sum};
	while(t--){
		gen_range();
		func[rnd() % func.size()]();
	}	
}

int main(){
	cin.sync_with_stdio(0);
	srand(time(0));
	clock_t s = clock();
	for (int _ = 10; _--;){
		unit_test(3000, 3000, ::rand);
	}
	clock_t t = clock();
	cerr << double(t - s) / CLOCKS_PER_SEC << " clear" << endl;
}
