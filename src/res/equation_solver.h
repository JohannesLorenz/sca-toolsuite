/*************************************************************************/
/* sca toolsuite - a toolsuite to simulate cellular automata.            */
/* Copyright (C) 2011-2019                                               */
/* Johannes Lorenz                                                       */
/* https://github.com/JohannesLorenz/sca-toolsuite                       */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#ifndef EQUATION_SOLVER_H
#define EQUATION_SOLVER_H

#include <cmath>
#include <cstring>
#include <cstdint>
#include <tuple>
#include <algorithm>
#include <limits>

//#include <boost/spirit/include/support_info.hpp>
//#include <boost/phoenix/bind/bind_function.hpp>

#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/proto/operators.hpp>

#include "random.h"

// typedef expression_ast result_type; // TODO: must work with phoenix 3!

//! this is a temporary fix until boost 1.57 - fixes a phoenix bug
namespace boost { namespace phoenix {
	using proto::exprns_::operator%=;
}}

//! namespace of the equation solver
namespace eqsolver
{
inline int f1i_not(int arg1) { return (int)(arg1==0); }
inline int f1i_neg(int arg1) { return -arg1; }
inline int f1i_abs(int arg1) { return std::abs(arg1); }
inline int f1i_sqrt(int arg1) { return (int)std::sqrt((int)arg1); }
inline int f1i_rand(int arg1) { return (int)sca_random::get_int(arg1); }
inline int f2i_add(int arg1, int arg2) { return arg1 + arg2; }
inline int f2i_sub(int arg1, int arg2) { return arg1 - arg2; }
inline int f2i_lshift(int arg1, int arg2) { return (unsigned)arg1 << (unsigned)arg2; }
inline int f2i_rshift(int arg1, int arg2) { return(unsigned)arg1 >> (unsigned)arg2; }
inline int f2i_mul(int arg1, int arg2) { return arg1 * arg2; }
inline int f2i_div(int arg1, int arg2) { return arg1 / arg2; }
inline int f2i_mod(int arg1, int arg2) { return arg1 % arg2; }
inline int f2i_min(int arg1, int arg2) { return std::min(arg1, arg2); }
inline int f2i_max(int arg1, int arg2) { return std::max(arg1, arg2); }
inline int f2i_lt(int arg1, int arg2) { return (int)(arg1<arg2); }
inline int f2i_gt(int arg1, int arg2) { return (int)(arg1>arg2); }
inline int f2i_le(int arg1, int arg2) { return (int)(arg1<=arg2); }
inline int f2i_ge(int arg1, int arg2) { return (int)(arg1>=arg2); }
inline int f2i_eq(int arg1, int arg2) { return (int)(arg1==arg2); }
inline int f2i_neq(int arg1, int arg2) { return (int)(arg1!=arg2); }
inline int f2i_and(int arg1, int arg2) { return (int)(arg1*arg2); }
inline int f2i_or(int arg1, int arg2) { return (int)((arg1!=0)||(arg2!=0)); }
inline int f2i_land(int arg1, int arg2) { return (unsigned)arg1 & (unsigned)arg2; }
inline int f2i_lor(int arg1, int arg2) { return (unsigned)arg1 | (unsigned)arg2; }
inline int f2i_lxor(int arg1, int arg2) { return (unsigned)arg1 ^ (unsigned)arg2; }
inline int f2i_asn(int* arg1, int arg2) { return (*arg1 = arg2); }
inline int f2i_com(int arg1, int arg2) { (void)arg1; return arg2; }
inline int f3i_tern(int arg1, int arg2, int arg3) { return((bool)arg1)?arg2:arg3; }

// typedef int (*fptr_1i)(int arg1);
// typedef int (*fptr_2i)(int arg1, int arg2);

template<class Ret, class ...Args>
using fptr_base = Ret (*)(Args...);

// using fptr_1i = fptr_base<int, int>;
// using fptr_2i = fptr_base<int, int, int>;

//namespace qi = boost::spirit::qi;
//namespace ascii = boost::spirit::ascii;

template<class Ret, class ...Args>
struct nary_op;

template<std::size_t... Is> struct seq {};
template<std::size_t N, std::size_t... Is> struct make_seq : make_seq<N-1, N-1, Is...> {};
template<std::size_t... Is> struct make_seq<0, Is...> : seq<Is...> {};

class visit_result_type
{
	boost::variant<unsigned int, int*> v;
public:
	visit_result_type(unsigned int i) : v(i) {}
	visit_result_type(int* i) : v(i) {}

	// templates assure that the 1st version will be preferred
	//template<class T> visit_result_type(T& i) : v(i) {}
	//template<> visit_result_type(int* i) : v(i) {}

	visit_result_type() {}
	operator int() { return boost::get<unsigned int>(v); }
	operator int*() { return boost::get<int*>(v); }
};

//! empty expression, can't happen!
struct nil {};

struct vaddr
{
	struct var_x{}; // TODO: 0-dimensional case? specialization?
	struct var_y{};

	template<bool IsAddress, char Sign, class ...Coords>
	class var_vector
	{
		std::tuple<Coords...> coords;
	public:
		//!< direct ctor
		var_vector(Coords ...coords) : coords(coords...) {}
		//!< ctor from std::string
		template<class ...String>
		var_vector(String const& ...strings) : var_vector(atoi(strings.c_str())...) {}
		template<char Idx>
		int x() const { return std::get<Idx-1>(coords); }
		std::string to_str() const { return "TODO"; }
	};

	template<bool Addr>
	using var_array = var_vector<Addr, 'a', int, int>;
	template<bool Addr>
	using var_helper = var_vector<Addr, 'h', int>;

	typedef boost::variant<nil, var_x, var_y, var_array<true>, var_array<false>, var_helper<true>, var_helper<false>> type;
	type expr;
	vaddr(const type& _t) { expr = _t; }
	vaddr& operator=(const type& _expr) { expr = _expr; return *this; }
	vaddr() : expr(nil()) {}
};

struct grid_storage_nothing
{
	// TODO: deduce unsigned int / int* from var_array?
	inline unsigned int operator()(vaddr::var_array<false> ) const {
		return std::numeric_limits<int>::min(); }
	inline int* operator()(vaddr::var_array<true> ) const {
		return nullptr; }
};

class grid_storage_base
{
	int width;
protected:
	grid_storage_base(int width) : width(width) {}
	template<bool Addr>
	inline int idx(const vaddr::var_array<Addr>& _a) const {
		return _a.template x<1>() + _a.template x<2>() * width; // TODO: give var_array a constexpr cross product with (1,width)?
	}
};

class const_grid_storage_array : grid_storage_base
{
	using storage_t = const int*;
	storage_t const v;
public:
	const_grid_storage_array(storage_t const v, int width) :
		grid_storage_base(width), v(v) {}
	inline unsigned int operator()(vaddr::var_array<false> _a) const {
		return v[idx(_a)]; }
};

class grid_storage_array : grid_storage_base
{
	using storage_t = int*;
	storage_t const v;
public:
	grid_storage_array(storage_t const v, int width) :
		grid_storage_base(width), v(v) {}
	inline int* operator()(vaddr::var_array<true> _a) const { // TODO: ref?
		return v + idx(_a); }
	inline unsigned int operator()(vaddr::var_array<false> _a) const {
		return v[idx(_a)]; }
};

class grid_storage_single
{
	using storage_t = int*;
	storage_t const v;
	template<bool T>
	void bounds_check(const vaddr::var_array<T>& _a) const {
		if(_a.template x<1>() || _a.template x<2>())
		 throw "Error: This CA only supports writing to the center cell a[0,0]";
	}
public:
	grid_storage_single(storage_t const v) : v(v) {}
	inline unsigned int operator()(vaddr::var_array<false> _a) const {
		return bounds_check(_a), *v;
	}
	inline int* operator()(vaddr::var_array<true> _a) const { // TODO: ref?
		return bounds_check(_a), v;
	}
};

class grid_storage_bits : grid_storage_base
{
	using storage_t = int64_t;
	const storage_t each, bitmask;
	const storage_t grid;
	char vpos; //!< position ("nth bit")
public:
	inline unsigned int operator()(vaddr::var_array<false> _a) const {
		return (grid >> ((vpos + idx(_a)) * each)) & bitmask;
	}
	inline int* operator()(vaddr::var_array<true> ) const {
		return nullptr; // TODO! ref class
	}
	grid_storage_bits(storage_t grid, storage_t each, int width, storage_t vpos) :
		grid_storage_base(width),
		each(each),
		bitmask((1<<each)-1),
		grid(grid),
		vpos(vpos)
	{

	}
};

template<class GridStorageSrc = const_grid_storage_array,
	class GridStorageTar = grid_storage_single>
class _variable_print : public boost::static_visitor<visit_result_type>
{
public:
	_variable_print(int _x) : x(_x) {}
	/**
	 * @brief variable_print
	 * @param _height
	 * @param _width
	 * @param _x
	 * @param _y
	 * @param _v The address of the current element in the grid. Needed for neighbor calculations.
	 * @param _h
	 */
	_variable_print(int _x, int _y, const GridStorageSrc& src,
		const GridStorageTar& tar,
		const int* _h)
		: /*height(_height), width(_width),*/ x(_x), y(_y),
		src(src), tar(tar),
		/*v((const int*)_v), */helper_vars((int*)_h) {
	}

//	typedef unsigned int result_type;

//	int height, width; // TODO: height unused?
private:
	unsigned int x,y;
//	const int *v;
	GridStorageSrc src;
	GridStorageTar tar;
	int* helper_vars;
public:
	inline unsigned int operator()(nil) const { return 0; }
	inline result_type operator()(vaddr::var_x) const { return x; }
	inline result_type operator()(vaddr::var_y) const { return y; }
	inline unsigned int operator()(vaddr::var_array<false> _a) const {
		return src(_a);
	}
	inline int* operator()(vaddr::var_array<true> _a) const {
		return tar(_a);
	}
	inline int* operator()(vaddr::var_helper<true> _h) const {
		//return (_h.address) ? ((result_type*)(helper_vars + _h.i)) : helper_vars[_h.i];
		return helper_vars + _h.x<1>();
	}
	inline unsigned int operator()(vaddr::var_helper<false> _h) const {
		//return (_h.address) ? ((result_type*)(helper_vars + _h.i)) : helper_vars[_h.i];
		return helper_vars[_h.x<1>()];
	}

};

using variable_print = _variable_print<grid_storage_nothing, grid_storage_nothing>;

struct variable_area_grid : public boost::static_visitor<visit_result_type>
{
	inline unsigned int operator()(nil) const { return 0; }
	inline unsigned int operator()(vaddr::var_x) const { return 0; }
	inline unsigned int operator()(vaddr::var_y) const { return 0; }
	template<bool Addr>
	inline result_type operator()(vaddr::var_array<Addr> _a) const {
		return std::max(std::abs(_a.template x<1>()), std::abs(_a.template x<2>()));
	}
	template<bool T>
	inline unsigned int operator()(vaddr::var_helper<T> _h) const { (void)_h; return 0; }
};

template<class Cont>
struct variable_area_cont : public boost::static_visitor<Cont>
{
	using cont_t = Cont;
	using base = boost::static_visitor<cont_t>;
	using typename base::result_type;
	inline result_type operator()(nil) const { return cont_t(); }
	inline result_type operator()(vaddr::var_x) const { return cont_t(); }
	inline result_type operator()(vaddr::var_y) const { return cont_t(); }
	template<bool Addr>
	inline result_type operator()(vaddr::var_array<Addr> _a) const
	{
		using vt = typename cont_t::value_type;
		using coord_t = typename vt::coord_t;
		return cont_t {{ static_cast<coord_t>(_a.template x<1>()),
			static_cast<coord_t>(_a.template x<2>())}};
	}
	template<bool T>
	inline result_type operator()(vaddr::var_helper<T> ) const { return cont_t(); }
};


struct variable_area_helpers : public boost::static_visitor<visit_result_type>
{
	inline int operator()(nil) const { return -1; }
	inline result_type operator()(vaddr::var_x) const { return -1; }
	inline result_type operator()(vaddr::var_y) const { return -1; }
	template<bool Addr>
	inline result_type operator()(vaddr::var_array<Addr> _a) const { (void)_a; return -1; }

	inline result_type operator()(vaddr::var_helper<true> _h) const { return _h.x<1>(); } // todo: correct?
	inline result_type operator()(vaddr::var_helper<false> _h) const { return _h.x<1>(); } // todo: correct?
};

//! expression tree node
class expression_ast
{
/*	using type = boost::variant<
		nil
		, unsigned int
		, std::string  // TODO: needed?
		, vaddr
		, boost::recursive_wrapper<expression_ast>
		, boost::recursive_wrapper<binary_op<Ret, Args...>>
		, boost::recursive_wrapper<unary_op<Ret, Args...>>
		>;*/
public:
	using type = boost::variant<
		nil
		, unsigned int
		, std::string  // TODO: needed?
		, vaddr
		, boost::recursive_wrapper<expression_ast>
		, boost::recursive_wrapper<nary_op<int, int, int, int>>
		, boost::recursive_wrapper<nary_op<int, int, int>>
		, boost::recursive_wrapper<nary_op<int, int*, int>>
		, boost::recursive_wrapper<nary_op<int, int>>
		>;

	expression_ast() : expr(nil()) {}
	expression_ast(expression_ast const& ai) : expr(ai.expr) {}
	explicit expression_ast(type const& ai) : expr(ai) {}
	template <typename Expr>
	explicit expression_ast(Expr const& expr) : expr(expr) {}

	template <typename T> struct factory_f {
		template<typename> struct result { typedef T type; };
		template<typename A> T operator()(A const& a) const { return T(a); }
	};

	expression_ast& operator<<(expression_ast const& rhs);
	expression_ast& operator>>(expression_ast const& rhs);

	expression_ast& operator+=(expression_ast const& rhs);
	expression_ast& operator-=(expression_ast const& rhs);

	expression_ast& operator*=(expression_ast const& rhs);
	expression_ast& operator/=(expression_ast const& rhs);
	expression_ast& operator%=(expression_ast const& rhs);

	expression_ast& operator<(expression_ast const & rhs);
	expression_ast& operator>(expression_ast const & rhs);
	expression_ast& operator<=(expression_ast const & rhs);
	expression_ast& operator>=(expression_ast const & rhs);

	expression_ast& operator==(expression_ast const & rhs);
	expression_ast& operator!=(expression_ast const & rhs);

	expression_ast& operator&&(expression_ast const & rhs);
	expression_ast& operator||(expression_ast const & rhs);

	expression_ast& operator&(expression_ast const & rhs);
	expression_ast& operator|(expression_ast const & rhs);
	expression_ast& operator^(expression_ast const & rhs);

	type expr; // TODO: make private + get func?
};

//! expression tree node extension for ternary operators
template<class Ret, class ...Args>
struct nary_op
{
	// below, (*) is a little fix to allow
	// expression_ast(type const& ai) to be explicit
	template<class ...AstClass>
	nary_op(
		char op
		, fptr_base<Ret, Args...> fptr
		, expression_ast::type const& left // (*)
		, AstClass const& ... more)
	: op(op), fptr(fptr), subtrees{(expression_ast)left, more...} {
		static_assert(sizeof...(AstClass) + 1 == sizeof...(Args),
			"Invalid number of args for nary_op ctor.");
	}

	char op;
	fptr_base<Ret, Args...> fptr;
	expression_ast subtrees[sizeof...(Args)];
};

inline int array_subscript_to_coord(std::string* str, int width)
{
	const int comma_pos = str->find(',');
	const int first_pos = str->find('[')+1;
	(*str)[comma_pos]=0;
	(*str)[str->find(']')]=0;
	//printf("coords: %s, %s\n",str->data()+first_pos, str->data()+(comma_pos+1));
	return atoi(str->data()+first_pos)+atoi(str->data()+(comma_pos+1))*width;
}

//! Class for iterating an expression tree and printing the result.
template<typename variable_handler>
class ast_print  : public boost::static_visitor<visit_result_type>
{
	// todo: only one possible variable_handler? -> then put it in here
	// ( and don't alloc direct access from outside?)

	const variable_handler* var_print;

	// h and w shall be internal, since we want to avoid adding 2
	// so it is still general to non-bordered areas
//	int height, width;

public:
	typedef unsigned int res_type;
//	const res_type x,y, *v;
	//mutable std::map<int, int> helper_vars;
//	variable_print var_print;

//	inline int position(int _x, int _y) const { return (_y*(width+1)+_x+1); }

	inline res_type operator()(nil) const { return 0; }
	inline res_type operator()(int n) const { return n; }
	inline res_type operator()(std::string) const
	{
		exit(99);
	}

	inline visit_result_type operator()(const vaddr& v) const
	{
		return boost::apply_visitor(*var_print, v.expr);
	}

	inline visit_result_type operator()(expression_ast const& ast) const
	{
		//ast_area helper_num(ast_area::MAX_HELPER);
		//helper_vars = new int[helper_num()+1];
		// TODO: delete int[]
		return boost::apply_visitor(*this, ast.expr);
	}
private:
	template<class NaryOpT, std::size_t ...Idxs>
	inline res_type apply_fptr(NaryOpT const& expr, seq<Idxs...>) const
	{
		// the temporary variable assures that the
		// apply order is correct (at least in C++11)
		result_type res[sizeof...(Idxs)] = { // TODO: make this const?
			boost::apply_visitor(*this, expr.subtrees[Idxs].expr)... };
	/*	std::cerr << boost::get<nary_op<int, int>>(expr.subtrees[0].expr).op << std::endl;*/
		return expr.fptr(res[Idxs]...);
	}

public:
	template<class Ret, class ...Args>
	inline res_type operator()(nary_op<Ret, Args...> const& expr) const
	{
		return apply_fptr(expr, make_seq<sizeof...(Args)>());
	}

	ast_print(const variable_handler* _var_print) : var_print(_var_print) {}

//	ast_print(int x, int y, int* v) : x(x), y(y), v((result_type*)v) {}

};

namespace dump_detail
{


	template<class ...StrClass>
	inline std::string apply(const std::string& str1, const StrClass&... str_list)
	{
		return str1 + ", " + apply(str_list...);
	}

	template<>
	inline std::string apply<>(const std::string& str1)
	{
		return str1;
	}

}

//! Class for iterating an expression tree and printing the result.
class ast_dump  : public boost::static_visitor<std::string>
{
	const char* const err_str = "<error>"; // TODO: static
public:
	using result_type = std::string;
	struct variable_handler : public boost::static_visitor<std::string>
	{
		const char* const err_str = "<error>"; // TODO: use parent class's err str
		inline result_type operator()(nil) const { return err_str; }
		inline result_type operator()(vaddr::var_x) const { return "x"; }
		inline result_type operator()(vaddr::var_y) const { return "y"; }
		template<bool Addr>
		inline result_type operator()(vaddr::var_array<Addr> _a) const { return _a.to_str(); }

		template<bool Value>
		inline result_type operator()(vaddr::var_helper<Value> _h) const { return _h.to_str(); } // todo: correct?

		variable_handler(){}
	};

	const variable_handler var_dump;

	// h and w shall be internal, since we want to avoid adding 2
	// so it is still general to non-bordered areas
//	int height, width;
	//typedef unsigned int res_type;
//	const res_type x,y, *v;
	//mutable std::map<int, int> helper_vars;
//	variable_print var_print;

//	inline int position(int _x, int _y) const { return (_y*(width+1)+_x+1); }

	inline result_type operator()(nil) const { return err_str; }
	inline result_type operator()(int n) const { return std::to_string(n); }
	inline result_type operator()(std::string) const
	{
		exit(99);
	}

	inline result_type operator()(const vaddr& v) const
	{
		return boost::apply_visitor(var_dump, v.expr);
	}

	inline result_type operator()(expression_ast const& ast) const
	{
		//ast_area helper_num(ast_area::MAX_HELPER);
		//helper_vars = new int[helper_num()+1];
		// TODO: delete int[]
		return boost::apply_visitor(*this, ast.expr);
	}
private:
	template<class NaryOpT, std::size_t ...Idxs>
	inline result_type apply_fptr(NaryOpT const& expr, seq<Idxs...>) const
	{
		result_type res[sizeof...(Idxs)] = {
			boost::apply_visitor(*this, expr.subtrees[Idxs].expr)... };
		return dump_detail::apply(res[Idxs]...);
	}

public:
	template<class Ret, class ...Args>
	inline result_type operator()(nary_op<Ret, Args...> const& expr) const
	{
		return apply_fptr(expr, make_seq<sizeof...(Args)>());
	}

};

//! computes int max of n values
template<class ...IntT>
inline int n_max(int first, IntT ...more) {
	return std::max(first, n_max(more...));
}

template<>
inline int n_max(int first) {
	return first;
}

//! Class for iterating an expression tree and print the used area in the array.
//! The result is an int describing the half size of a square.
template<typename variable_handler>
class ast_area  : public boost::static_visitor<unsigned int>
{
	variable_handler var_area;
public:
	typedef unsigned int result_type;

	inline result_type operator()(nil) const { return 0; }
	inline result_type operator()(int) const { return 0;  }
	inline result_type operator()(std::string) const
	{
		exit(99);
	}

	inline result_type operator()(const vaddr& v) const
	{
		return boost::apply_visitor(var_area, v.expr);
	}

	inline result_type operator()(expression_ast const& ast) const {
		return boost::apply_visitor(*this, ast.expr);
	}

private:
	template<class NaryOpT, std::size_t ...Idxs>
	inline result_type apply_fptr(NaryOpT const& expr, seq<Idxs...>) const
	{
		return n_max(
			(int)boost::apply_visitor(*this, expr.subtrees[Idxs].expr)...
			// TODO: this cast is dangerous!
			// (the result is sometimes a pointer, somehow)
		);
	}
public:
	template<class Ret, class ...Args>
	inline result_type operator()(nary_op<Ret, Args...> const& expr) const {
		return apply_fptr(expr, make_seq<sizeof...(Args)>());
	}

	//ast_area(variable_area::AREA_TYPE _area_type)
	//	: var_area(_area_type) {}
};

//! Class for iterating an expression tree and print the used area in the array.
//! The result is an int describing the half size of a square.
template<typename variable_handler>
class ast_area_cont : public boost::static_visitor<typename variable_handler::cont_t>
{
	variable_handler var_area;
public:
	using cont_t = typename variable_handler::cont_t;
	using base = boost::static_visitor<cont_t>;
	using typename base::result_type;
	//typedef unsigned int result_type;

	inline result_type operator()(nil) const { return cont_t(); }
	inline result_type operator()(int) const { return cont_t(); }
	inline result_type operator()(std::string) const
	{
		exit(99);
	}

	inline result_type operator()(const vaddr& v) const
	{
		return allow ? boost::apply_visitor(var_area, v.expr)
			: cont_t();
	}

	inline result_type operator()(expression_ast const& ast) const {
		return boost::apply_visitor(*this, ast.expr);
	}

private:
	bool input;
	mutable bool allow;

	// TODO: this is mostly a duplicate... dumbly solved!
	template<class NaryOpT> // no indexes
	inline void apply_fptr(cont_t& , NaryOpT const&) const
	{
	}

	// TODO: this is mostly a duplicate... dumbly solved!
	template<class NaryOpT, std::size_t idx0, std::size_t ...Idxs>
	inline void apply_fptr(cont_t& res, NaryOpT const& expr) const
	{
		cont_t to_append =
			boost::apply_visitor(*this, std::move(expr.subtrees[idx0].expr));
		std::move(to_append.begin(), to_append.end(),
			std::inserter(res, res.begin()));
		apply_fptr<NaryOpT, Idxs...>(res, expr);
	}

	template<class NaryOpT, std::size_t ...Idxs>
	inline void apply_fptr(cont_t& res, NaryOpT const& expr, seq<Idxs...>) const
	{
		apply_fptr<NaryOpT, Idxs...>(res, expr);
	}
public:
	template<class Ret, class ...Args>
	inline result_type operator()(nary_op<Ret, Args...> const& expr) const
	{
		cont_t res;
		if((void*)expr.fptr == (void*)f2i_asn) // TODO: this is not clean
		{
			if(!input)
			{ // apply left side specially for output
				allow = true;
				res = boost::apply_visitor(*this, std::move(expr.subtrees[0].expr));
				allow = false;
			}
			apply_fptr<nary_op<Ret, Args...> const&, 1>(res, expr); // ignore left side
		}
		else
		 apply_fptr(res, expr, make_seq<sizeof...(Args)>());
		return res;
	}

	//ast_area(variable_area::AREA_TYPE _area_type)
	//	: var_area(_area_type) {}
	//! @param input true if shall measure input n, false for output
	ast_area_cont(bool input) : input(input), allow(input) {}
};

#if 0
namespace minmax_detail
{
	typedef std::pair<int, int> expr_pair;
	typedef std::pair<expression_ast, expression_ast> int_pair;

	// maps pair (min, max) to those number that are only true or false
/*	int_pair make_logic_pair()
	{
		int_pair result {0, 1};
		if(min > 0 || max < 0)
		{
			result.first = 1;
		}
		else if(min == 0 && max == 0)
		{
			result.second = 0;
		}
		return result;
	}*/
/*
	struct logic_pair
	{
		std::pair p;
		logic_pair()
	};*/

	/*class logic_pair
	{
		int_pair p; // TODO: make const
	public:
		//logic_pair(const int_pair& in) : p(in) {}
		logic_pair(const int_pair& in)
		{
			p = {false, true};
			if(in.first > 0 || in.second < 0)
			{
				p.first = true;
			}
			else if(in.first == 0 && in.second == 0)
			{
				p.second = false;
			}
		}

		int_pair operator!() const
		{
			int_pair result = {!p.first, !p.second};
			using std::swap;
			if(result.first > result.second)
			 swap(result.first, result.second);
			return result;
		}

		int_pair operator&&(const logic_pair& other) const
		{
			return int_pair(other.p.first && p.first, other.p.second && p.second);
		}


		bool always_true() const { return p.first && p.second; }
		bool always_false() const { return !(p.first || p.second); }

	};*/

	template<int DefaultValue>
	struct get_ints_t : public boost::static_visitor<int>
	{
		inline result_type operator()(int n) { return n; }
		template<class T>
		inline result_type operator()(const T& ) { return DefaultValue; }
	};

	get_ints_t<INT_MIN> min_getter;
	get_ints_t<INT_MAX> max_getter;


	//void get_ints(std::pair<expression_ast, expression_ast>)
	int get_min_int(const expression_ast& e) {
		return boost::apply_visitor(min_getter, e);
	}

	int get_max_int(const expression_ast& e) {
		return boost::apply_visitor(max_getter, e);
	}

	std::pair<int, int> get_int_pair(const expression_ast& e) {
		return { get_min_int(i), get_max_int(e) };
	}

	class logic_pair
	{
		bool poss0 = true, poss1 = true;

		int_pair pair_from_poss(bool _poss0, bool _poss1) const
		{
			return _poss0
				? (_poss1 ? int_pair{0,1} : int_pair{0,0})
				: int_pair{1,1} ;
		}

	public:
		logic_pair(const int_pair& in)
		{
			if(in.first > 0 || in.second < 0)
			{
				poss0 = false;
			}
			else if(in.first == 0 && in.second == 0)
			{
				poss1 = false;
			}
		}

		int_pair operator!() const
		{
			return pair_from_poss(poss1, poss0);
		}
		int_pair operator&&(const logic_pair& other) const
		{
			/*return (poss1 && other.poss1)
				?*/
			bool _poss1 = poss1 && other.poss1;
			bool _poss0 = poss0 || other.poss0;
			return pair_from_poss(_poss0, _poss1);
		}
		int_pair operator||(const logic_pair& other) const
		{
			/*return (poss1 && other.poss1)
				?*/
			bool _poss1 = poss1 || other.poss1;
			bool _poss0 = poss0 && other.poss0;
			return pair_from_poss(_poss0, _poss1);
		}

		bool always_true() const { return !poss0; }
		bool always_false() const { return !poss1; }
	};

	/*template<class NaryOpT, class ...Types>
	int_pair apply(NaryOpT const& expr, Types...)
	{
		(void) expr;
		return std::pair<int, int>(42, 37);
	}*/

	inline expr_pair apply(nary_op<int, int> const& expr, expr_pair p1)
	{
		// TODO: switch with ids?
		// TODO: layer this out in other classes?
		// TODO: maybe templates for the pointer (binary size!)
		const auto& fptr = expr.fptr;

		int_pair ip = get_int_pair(expr);


		if(ip.first == INT_MIN && ip.second == INT_MAX)
		{


		}
		else
		{
		if(fptr == f1i_not)
		{
		//	return std::pair<int, int>(0, 1);
			return !logic_pair(ip);
		}
		else if(fptr == f1i_neg)
		{
			// note that this is not the else case
			return std::pair<int, int>(-ip.second, -ip.first);
		}
		else if(fptr == f1i_abs)
		{
			// this is easy to proove if you think about the
			// function's plot
			return std::pair<int, int>(
				std::max(std::min(p1.first, -p1.second), 0),
				std::max(p1.second, -p1.first));
		}
		else if(fptr == f1i_sqrt)
		{
			// we assume that the user does not sqrt() negative
			// values. if they'll be needed, we should add nqsrt()
			return std::pair<int, int>(0, fptr(p1.second));
		}
		else if(fptr == f1i_rand)
		{
			return std::pair<int, int>(0, p1.second);
		}
		else
		 assert(false);
		}
	}

	inline expr_pair apply(nary_op<int, int, int, int> const& expr, expr_pair p1, expr_pair p2, expr_pair p3)
	{
		const auto& fptr = expr.fptr;

		if(fptr == f3i_tern)
		{
			logic_pair l(p1);
			if(l.always_true())
			 return p2;
			else if(l.always_false())
			 return p3;
			else
			 return int_pair{ std::min(p2.first, p3.first),
				std::max(p2.second, p3.second) } ;
		}
		else
		 assert(false);
	}

	inline int_pair apply(nary_op<int, int, int> const& expr, int_pair p1, int_pair p2)
	{
#if 0
	inline int f2i_add(int arg1, int arg2) { return arg1 + arg2; }
inline int f2i_sub(int arg1, int arg2) { return arg1 - arg2; }
inline int f2i_mul(int arg1, int arg2) { return arg1 * arg2; }
inline int f2i_div(int arg1, int arg2) { return arg1 / arg2; }
inline int f2i_mod(int arg1, int arg2) { return arg1 % arg2; }
inline int f2i_min(int arg1, int arg2) { return std::min(arg1, arg2); }
inline int f2i_max(int arg1, int arg2) { return std::max(arg1, arg2); }
inline int f2i_lt(int arg1, int arg2) { return (int)(arg1<arg2); }
inline int f2i_gt(int arg1, int arg2) { return (int)(arg1>arg2); }
inline int f2i_le(int arg1, int arg2) { return (int)(arg1<=arg2); }
inline int f2i_ge(int arg1, int arg2) { return (int)(arg1>=arg2); }
inline int f2i_eq(int arg1, int arg2) { return (int)(arg1==arg2); }
inline int f2i_neq(int arg1, int arg2) { return (int)(arg1!=arg2); }
inline int f2i_and(int arg1, int arg2) { return (int)(arg1*arg2); }
inline int f2i_or(int arg1, int arg2) { return (int)((arg1!=0)||(arg2!=0)); }
inline int f2i_asn(int* arg1, int arg2) { return (*arg1 = arg2); }
inline int f2i_com(int arg1, int arg2) { (void)arg1; return arg2; }
#endif

		// TODO: switch with ids?
		// TODO: layer this out in other classes?
		// TODO: maybe templates for the pointer (binary size!)
		const auto& fptr = expr.fptr;

		if(fptr == f2i_eq)
		{
			bool tmp;
			return ((p1.first == p1.second) && (p2.first == p2.second))
				? ( tmp = fptr(p1.first, p2.first), int_pair {tmp, tmp} )
				: int_pair{0, 1};
		}
		else if(fptr == f2i_and)
		{
			return logic_pair(p1) && logic_pair(p2);
		}
		else if(fptr == f2i_or)
		{
			return logic_pair(p1) || logic_pair(p2);
		}
		else if(fptr == f2i_com)
		{
			return p2;
		}
		else
		 assert(false);
	}

	// TODO: using int_pair_ref = const int_pair& ?
	inline int_pair apply(nary_op<int, int*, int> const& expr, int_pair* p1, int_pair p2)
	{
		const auto& fptr = expr.fptr;
		assert(fptr == f2i_asn);
		std::cout << "before : " << p1->first << std::endl;
		fptr(&p1->first, p2.first);
		std::cout << "after : " << p1->first << std::endl;
		return int_pair { fptr(&p1->first, p2.first), fptr(&p1->second, p2.second) };
		//return int_pair{42, 4};
	}

}
#endif

#if 0

//! Class for iterating an expression tree and print the used area in the array.
//! The result is an int describing the half size of a square.
struct ast_minmax : public boost::static_visitor<unsigned int>
{
//	typedef std::pair<int, int> result_type;
	using int_pair = std::pair<expression_ast, expression_ast>;
//	const static int_pair arbitrary;

	class mm_result_type
	{
		boost::variant<int_pair, int_pair*> v;
	public:
		typedef std::pair<expression_ast, expression_ast> int_pair;

		mm_result_type(int_pair i) : v(i) {}
		mm_result_type(int_pair* i) : v(i) {}

		// templates assure that the 1st version will be preferred
		//template<class T> visit_result_type(T& i) : v(i) {}
		//template<> visit_result_type(int* i) : v(i) {}

		mm_result_type() {}
		operator int_pair() { return boost::get<int_pair>(v); }
		operator int_pair*() { return boost::get<int_pair*>(v); }
	};

	using result_type = mm_result_type;

	struct variable_minmax_helpers : public boost::static_visitor<mm_result_type>
	{
		typedef std::pair<expression_ast, expression_ast> int_pair;
		int_pair* helper_vars;
		const expression_ast expr_x, expr_y, expr_v;
	public:
		inline int_pair operator()(nil) const { exit(99); }
		inline int_pair operator()(vaddr::var_x) const { return int_pair(expr_x, expr_x); }
		inline int_pair operator()(vaddr::var_y) const { return int_pair(expr_y, expr_y); }
		template<bool Addr>
		inline int_pair operator()(vaddr::var_array<Addr>) const { return int_pair(expr_v, expr_v); }
		// TODO: correctly done for Addr == true?

		inline int_pair* operator()(vaddr::var_helper<true> _h) const {
			//return (_h.address) ? ((result_type*)(helper_vars + _h.i)) : helper_vars[_h.i];
		//	std::cout << "returning ptr: " << helper_vars[_h.i].first << std::endl;
			return helper_vars + _h.x<1>();
		}
		inline int_pair operator()(vaddr::var_helper<false> _h) const {
			//return (_h.address) ? ((result_type*)(helper_vars + _h.i)) : helper_vars[_h.i];
		//	std::cout << "returning value: " << helper_vars[_h.i].first << std::endl;
			return helper_vars[_h.x<1>()];
		}

		variable_minmax_helpers(std::size_t helpers_size)
			:  expr_x(expression_ast(vaddr(vaddr::var_x()))),
			 expr_y(expression_ast(vaddr(vaddr::var_y()))),
			expr_v(expression_ast(vaddr(vaddr::var_array<true>(0, 0)))) // TODO: correct?
			 // TODO: should be static
		{
			helper_vars = new int_pair[helpers_size];
		}
		~variable_minmax_helpers() { delete[] helper_vars; }
	};

	variable_minmax_helpers var_minmax;

	ast_minmax(std::size_t helpers_size) : var_minmax(helpers_size) {}

	inline result_type operator()(nil) const { exit(99); }
	inline result_type operator()(int n) const { return int_pair{expression_ast(n), expression_ast(n)};  }
	inline result_type operator()(std::string) const
	{
		exit(99);
	}

	inline result_type operator()(const vaddr& v) const
	{
		return boost::apply_visitor(var_minmax, v.expr);
	}

	inline result_type operator()(expression_ast const& ast) const {
		return boost::apply_visitor(*this, ast.expr);
	}

private:
	template<class NaryOpT, std::size_t ...Idxs>
	inline result_type apply_fptr(NaryOpT const& expr, seq<Idxs...>) const
	{
		result_type res[sizeof...(Idxs)] = {
			boost::apply_visitor(*this, expr.subtrees[Idxs].expr)... };
#if 0
		return minmax_detail::apply(
			expr,
			res[Idxs]...
			// TODO: this cast is dangerous!
			// (the result is sometimes a pointer, somehow)
		);
	/*	expression_ast e1 (INT_MIN);
		expression_ast e2 (INT_MAX);*/
		return int_pair(e1, e2); // TODO!! apply...
#else
		(void)res;
		expression_ast e1 (INT_MIN);
		expression_ast e2 (INT_MAX);
		return int_pair(e1, e2);
#endif
	}
public:
	template<class Ret, class ...Args>
	inline result_type operator()(nary_op<Ret, Args...> const& expr) const {
		return apply_fptr(expr, make_seq<sizeof...(Args)>());
	}

	//ast_area(variable_area::AREA_TYPE _area_type)
	//	: var_area(_area_type) {}
};

#endif

const char* get_help_description();

/**
	Builds AST from equation which can be used later to solve it.
	Good if you have to compute the same equation multiple times.
	@param equation Equation string.
	@param ast Pointer to AST which will be assigned.
	@throw error strings on parse error
*/
void build_tree(const char* equation, eqsolver::expression_ast* ast);

//! Computes the result of the equation with @a x, @a y and @a z filled in.
//! @todo unused
//int solve(const char* equation, int x, int y, int v);

}

#endif // EQUATION_SOLVER_H
