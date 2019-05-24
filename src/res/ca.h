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

//! @file ca.h generic templates for use of different ca simulators

#ifndef CA_H
#define CA_H

#include <random>

#include "random.h"
#include "ca_basics.h"
#include "bitgrid.h"

namespace sca { namespace ca {

// TODO: more basic bass class on top? (probably not)
class input_ca
{
protected:
	using cell_t = typename def_cell_traits::cell_t;
	using u_coord_t = typename def_coord_traits::u_coord_t;
public:
	//! calculates next state at (human) position (x,y)
	//! if the ca is asynchronous, the function returns the next state
	//! as if the cell was active
	//! @param dim the grids *internal* dimension
	virtual int next_state(const int *cell_ptr, const point& p) const = 0;

	//! overload with human coordinates and reference to grid. slower.
	virtual int next_state(const point& p) const = 0;

	//! returns whether cell at point @a p will change if active
	//! @a result is set to the result in all cases, if it is not nullptr
	virtual bool is_cell_active(const point& p, cell_t* result = nullptr) const = 0;

	//! prepares the ca - *must* be called before the first run
	//! and *after* assigning the grid
	virtual void finalize() = 0;

	//! prepares the ca to run only on cells from @a sim_rect
	virtual void finalize(const rect& sim_rect) = 0;

	// TODO: function run_once_async()

	struct default_asynchronicity
	{
		bool operator()(unsigned ) const { return sca_random::get_int(2); }
	};

	struct synchronous
	{
		bool operator()(unsigned ) const { return true; }
	};

/*	//! runs the ca once, but only ever activating cells from @a sim_rect
	template<class Asynchronicity>
	virtual void run_once(const rect& sim_rect,
		const Asynchronicity& async = synchronous()) = 0;*/

	virtual void run_once(const rect& sim_rect) = 0;
	virtual void run_once(const rect& sim_rect, const default_asynchronicity& async) = 0;

	//! runs the whole ca
	//template<class Asynchronicity>
	virtual void run_once() = 0;
	virtual void run_once(const default_asynchronicity& async) = 0;

	virtual const std::vector<point>& active_cells() const = 0;
	virtual bool has_active_cells() const = 0;

	//! returns true iff not all cells are inactive
	virtual bool can_run() const = 0;

	//! returns true iff not all cells are inactive
	//virtual bool is_stable() const = 0;

	//! returns the current grid
	virtual grid_t& grid() = 0;
	virtual const grid_t& grid() const = 0;

	enum class stable_t
	{
		always,
		unknown,
		never
	};

	//! whether we know if the ca will get stable
	//! @todo: give asynch as param
	virtual stable_t gets_stable() const = 0;

	//! if gets_stable() returns always, this will get to an end
	//! configuration. if it returns never, this must return false.
	//! otherwise, the computation can take forever
	virtual bool stabilize() = 0;

	virtual void input(const point& p) = 0;

	virtual u_coord_t border_width() const = 0;

	virtual ~input_ca() {}
};

/**
 * @brief This class holds anything a cellular automaton's function
 * needs to know.
 *
 * Thus it contains no grid.
 */
template<class Solver, class Traits, class CellTraits>
class _calculator_t : public Solver
{
	using _base = Solver;
	using u_coord_t = typename Traits::u_coord_t;
	using cell_t = typename CellTraits::cell_t;
	using grid_t = _grid_t<Traits, CellTraits>;
	using point = _point<Traits>;
	using dimension = _dimension<Traits>;

public:
	using n_t = _n_t<Traits, std::vector<point>>;

private:
	const u_coord_t _border_width;
	const n_t _n_in, _n_out, _n_dep;
	const point cc_out;
	mutable grid_t tmp_out_grid;

	n_t calc_n_dep() const {
		return n_t(_n_in).append(_n_out);
	}

public:
	// TODO: single funcs to initialize and make const?
	// aka: : ast(private_build_ast), ...
	_calculator_t(const char* equation, unsigned num_states = 0) :
		Solver(equation, num_states),
		_border_width(_base::template calc_border_width<Traits>()),
		_n_in(_base::template calc_n_in<Traits>()),
		_n_out(_base::template calc_n_out<Traits>()),
		_n_dep(calc_n_dep()),
		cc_out(_n_out.center()),
		tmp_out_grid(_n_out.dim(), 0)
	{
	}

	// TODO: duplicate ctors -> bad!
	//! tries to synch (TODO: better word) the CA from a file
	_calculator_t(std::istream& stream) :
		Solver(stream),
		_border_width(_base::template calc_border_width<Traits>()),
		_n_in(_base::template calc_n_in<Traits>()),
		_n_out(_base::template calc_n_out<Traits>()),
		_n_dep(calc_n_dep()),
		cc_out(_n_out.center()),
		tmp_out_grid(_n_out.dim(), 0)
	{
	}

	const u_coord_t& border_width() const noexcept { return _border_width; }
	const n_t& n_in() const noexcept { return _n_in; }
	const n_t& n_out() const noexcept { return _n_out; }
	const n_t& n_dep() const noexcept { return _n_dep; }

	//! calculates next state at (human) position (x,y)
	//! @param dim the grids internal dimension
	int next_state_old(const cell_t *cell_ptr, const point& p, const dimension& dim) const // TODO: return cell_t
	{
		return _base::template calculate_next_state_old<Traits, CellTraits>(cell_ptr, p, dim);
	}

	//! calculates next states around (human) position (x,y)
	//! @param dim the grids internal dimension
	int next_state(const cell_t *cell_ptr, const point& p, const dimension& dim,
		cell_t *cell_tar, const dimension& tar_dim) const
	{
		return _base::template calculate_next_state<Traits, CellTraits>(cell_ptr, p, dim, cell_tar, tar_dim);
	}

	//! returns bitgrid
	bool next_state(const cell_t *cell_ptr, const point& p, const dimension& dim, bitgrid_t& res) const
	{
		return _base::template calculate_next_state<Traits, CellTraits>(cell_ptr, p, dim, res);
	}

	//! returns bitgrid
	bool next_state(const grid_t &grid, const point& p, bitgrid_t& res) const
	{
		return next_state(&grid[p], p, grid.internal_dim(), res);
	}

	//! overload, with x and y in internal format. slower.
	int next_state_realxy(const cell_t *cell_ptr, const point& p, const dimension& dim) const
	{
		int bw = _base::border_width();
		return next_state(cell_ptr, p - point { bw, bw }, dim);
	}

	//! overload with human coordinates and reference to grid. slower.
	int next_state_old(const grid_t &grid, const point& p) const
	{
		return next_state_old(&grid[p], p, grid.internal_dim());
	}

/*	//! overload with human coordinates and reference to grid. slower.
	int next_state(const grid_t &grid, const point& p, const grid_t & tar) const
	{
		return next_state(&grid[p], p, grid.internal_dim(), &tar[p], tar.internal_dim());
	}*/

#if 0
	//! returns whether cell at point @a p is active.
	//! @a result is set to the result in all cases, if it is not nullptr
	// TODO: overloads
	// TODO: deprecated
	bool is_cell_active(const grid_t& grid, const point& p, cell_t* result = nullptr) const
	{
		const cell_t* const cell_ptr = &grid[p];
		const cell_t next = next_state(cell_ptr, p, grid.internal_dim());
		if(result)
		 *result = next;
		return next != *cell_ptr;
	}
#endif

	//! calculates next states around (human) position (x,y)
	//! @param dim the grids internal dimension
	const grid_t& next_state_ref(const grid_t &grid, const point& p) const
	{
		next_state(&grid[p], p, grid.internal_dim(),
			&tmp_out_grid[cc_out], tmp_out_grid.internal_dim());
		return tmp_out_grid;
	}


	//! returns whether cell at point @a p is active.
	//! @a result is set to the result in all cases
	// TODO: overloads
	// TODO: deprecated -> ? -> probably not anymore...
	bool is_cell_active(const grid_t& grid, const point& p, grid_t*& result) const
	{
		// TODO: might be redirected to Solver to save time, in many cases

		// let next_state... return bool if it was in range?

		next_state_ref(grid, p); // TODO!!! result = &next ... ??
		result = &tmp_out_grid;

		// TODO: linewise compare?
		bool equal = true;
		for(auto itr = _n_out.cbegin();
			equal && itr != _n_out.cend(); ++itr)
		{
			const point grid_point = p + *itr;
			equal = equal && (
				grid[grid_point] == tmp_out_grid[cc_out + *itr]);
			if(! grid.contains(grid_point)) // TODO: cleaner abortion...
			 return false;
		}

	/*	if(!equal)
		{
			std::cout << "ACTIVE!" << p << std::endl;
			std::cout << grid << ", " << std::endl << tmp_out_grid << std::endl;
		}*/
		return !equal;
	}

	//! overload without result param
	bool is_cell_active(const grid_t& grid, const point& p) const
	{
		grid_t* ptr;
		return is_cell_active(grid, p, ptr);
	}

	//! complexity: at most O(log(n))
	bool is_state_dead(const cell_t& state) const
	{
		return _base::is_dead(state);
	}

	std::size_t num_states() const { return _base::num_states(); }

};

//template<class Traits, class CellTraits>
//using calculator_t = _calculator_t<eqsolver_t, Traits, CellTraits>;

#if 0 // TODO: clang forbids this for some strange reason
class asm_synch_calculator_t
{
	static constexpr const std::array<point, 5> neighbour_points = {{{0,-1},{-1,0}, {0,0}, {1, 0}, {0,1}}};
	static constexpr const ca::n_t_constexpr<5> neighbours = ca::n_t_constexpr<5>(neighbour_points);
public:
	~asm_synch_calculator_t() { }

	asm_synch_calculator_t()
	{
	}

	static constexpr int get_border_width() { return 1; }

	//! calculates next state at (human) position (x,y)
	//! @param dim the grids internal dimension
	int next_state(const int *cell_ptr, const point& , const dimension& dim) const
	{
	/*	// TODO: replace &((*old_grid)[internal]) by old_value
		// and make old_value a ptr/ref?
		eqsolver::variable_print vprinter(dim.height(), dim.width(),
			p.x, p.y,
			cell_ptr, helper_vars);
		eqsolver::ast_print<eqsolver::variable_print> solver(&vprinter);
		return (int)solver(ast);*/
		// TODO: move to asm_basics?

		return (((*cell_ptr) & 3)
			+ (cell_ptr[1			]>>2)
			+ (cell_ptr[-1			]>>2)
			+ (cell_ptr[-dim.width()	]>>2)
			+ (cell_ptr[dim.width()	]>>2));

	//	return ((*cell_ptr) & 3) || ((cell_ptr[1]|cell_ptr[-1]|cell_ptr[-dim.width()]|cell_ptr[dim.width()])&3);
	}

	//! overload, with x and y in internal format. equally slow.
	int next_state_realxy(const int *cell_ptr, const point& p, const dimension& dim) const
	{
		return next_state(cell_ptr, p, dim);
	}

	//! overload with human coordinates and reference to grid. slower.
	int next_state(const grid_t &grid, const point& p) const
	{
		return next_state(&grid[p], p, grid.internal_dim());
	}

	//! returns whether cell at point @a p is active.
	//! @a result is set to the result in all cases, if it is not nullptr
	// TODO: overloads
	bool is_cell_active(const grid_t& grid, const point& p, cell_t* result = nullptr) const
	{
		const int* cell_ptr = &grid[p];
		/*cell_t next = next_state(cell_ptr, p, grid.dim());
		if(result)
		 *result = next;
		return next != *cell_ptr;*/
		// TODO: should a cell only be active if it changes its value?

		if(*cell_ptr & 4)
		{
			*result = next_state(cell_ptr, p, grid.internal_dim());
			return true;
		}
		return false;

		//return *cell_ptr & 4;
	}

	n_t_constexpr<5> get_neighbourhood() const
	{
		return neighbours;
	}

	// TODO: get states function
};
#endif

/**
 * Defines sequence: (stabilisation)((input)(stabilisation))*
 */
template<class Solver, class Traits, class CellTraits>
class simulator_t : /*private _ca_calculator_t<Solver>,*/ public input_ca
{
	using point = _point<Traits>;
	using calc_class = _calculator_t<Solver, Traits, CellTraits>;
	using grid_t = _grid_t<Traits, CellTraits>;

	using input_class = calc_class; //!< TODO: Solver class is enough

	calc_class ca_calc;
	input_class ca_input;

	using tmp_grid_t =
	_grid_t<typename grid_t::traits_t, cell_traits<grid_t>>;
	tmp_grid_t tmp_grid;

	grid_t _grid[3];
	grid_t *old_grid = _grid, *new_grid = _grid; // TODO: old grid const?
	typename calc_class::n_t n_in, n_out, n_dep; // TODO: const?
	std::vector<point> new_changed_cells;
	std::vector<point> change_order; // initialize with some size?
	//! temporary variable
	std::set<point> cells_to_check, cells_not_token; // TODO: use pointers here, like in grid
	int round = 0; //!< steps since last input
	bool async; // TODO: const?

	static constexpr const char* def_in_eq = "v:=v";

	void initialize_first() { run_once(); }

public:
	simulator_t(const char* equation, const char* input_equation,
		unsigned num_states, bool async = false) :
		ca_calc(equation, num_states),
		ca_input(input_equation, num_states),
		_grid{ca_calc.border_width(),
			ca_calc.border_width(),
			ca_calc.border_width()},
		n_in(ca_calc.n_in()),
		n_out(ca_calc.n_out()),
		n_dep(ca_calc.n_dep()),
		async(async)
	{
	}

	simulator_t(const char* equation, const char* input_equation,
		bool async = false) : // TODO: 2 ctors?
		simulator_t(equation, input_equation, 0, async)
	{
	}

	simulator_t(const char* equation,
		bool async = false) :
		simulator_t(equation, def_in_eq, async)
	{
	}

	simulator_t(std::istream& stream, const char* input_equation = def_in_eq,
		bool async = false) :
		ca_calc(stream),
		ca_input(input_equation, 0),
		_grid{ca_calc.border_width(),
			ca_calc.border_width(),
			ca_calc.border_width()},
		n_in(ca_calc.n_in()),
		n_out(ca_calc.n_out()),
		async(async)
	{
	}

	simulator_t(const simulator_t& ) = delete;

	virtual ~simulator_t() {}

	// TODO: there is no virtual function right now...
	void reset_ca(const char* equation, const char* input_equation)
	{
		ca_calc = calc_class(equation);
		ca_input = input_class(input_equation);
		grid().resize_borders(ca_calc.border_width());
		n_in = ca_calc.n_in();
		n_out = ca_calc.n_out();
	}

	// TODO: no function should take grid pointer

	//! calculates next state at (human) position (x,y)
	//! @param dim the grids internal dimension
	int next_state(const int *cell_ptr, const point& p) const
	{
		(void) cell_ptr;
		return (*new_grid)[p];
	}

	//! overload with human coordinates and reference to grid. slower.
	int next_state(const point& p) const
	{
		return (*new_grid)[p];
	}

	//! returns whether cell at point @a p is active.
	//! @a result is set to the result in all cases, if it is not nullptr
	// TODO: overloads
	// TODO: faster than scanning the vector?
	bool is_cell_active(const point& p, typename grid_t::value_type* result = nullptr) const
	{
		//return ca_calc.is_cell_active(grid(), p, result);
		//return ca_calc.is_cell_active(old_grid, p, result);
		if(result)
		 *result =  (*new_grid)[p];
		return (*old_grid)[p] == (*new_grid)[p];
	}

	grid_t& grid() { return *old_grid; }
	const grid_t& grid() const { return *old_grid; }

	//! prepares the ca to run only on cells from @a sim_rect
	void finalize(const _rect<Traits>& sim_rect)
	{
		// incorrect if we finalize later? (what is 0 and 1?)
		_grid[1] = _grid[0]; // fit borders
		_grid[2] = _grid[0], _grid[2].reset(0);

		grid_t fill(n_out.dim(), 0);
		tmp_grid = tmp_grid_t(_grid[0].human_dim(), ca_calc.border_width(), fill, fill);
		for(const point& p : tmp_grid.points()) // TODO: common routine
		for(const point& np : n_out)
		 tmp_grid[p + np];

		// make all cells active, but not those close to the border
		// TODO: make this generic for arbitrary neighbourhoods
	//	new_changed_cells.reserve(sim_rect.area());
		for( const point &p : sim_rect ) {
			// TODO: use active criterion if possible
			// TODO: otherwise, invariant can be broken...
			// TODO: (because these cells are not active)
			//for(const point np : n_in)
			if(ca_calc.is_cell_active(_grid[0], p))
			 new_changed_cells.push_back(p);
		}

		initialize_first();
	}

	//! prepares the ca - *must* be run before simulating
	void finalize() { finalize(_grid->human_dim()); }

	// TODO: function run_once_async()

	//! runs the ca once, but only ever activating cells from @a sim_rect
	template<class Asynchronicity>
	void _run_once(const rect& sim_rect,
		const Asynchronicity& async = synchronous())
	{
		// switch grids
		old_grid = _grid + ((round+1)&1);
		new_grid = _grid + ((round)&1);

		cells_to_check.clear();

		// adds the cell to cells_to_check if it is variable
		const auto add_cell_if_variable_and_async = [&](const point& p)
		{
			if(sim_rect.is_inside(p))
			{
				/*(*new_grid)[p] =*/

				grid_t& cur_res_grid = tmp_grid[p];
#if 0
				ca_calc.next_state
					(&((*old_grid)[p]),
						p, _grid->internal_dim(),
						&((*new_grid)[p]), _grid->internal_dim());
#else
				ca_calc.next_state
					(&((*old_grid)[p]),
						p, _grid->internal_dim(),
						&cur_res_grid[n_out.center()], n_out.dim());
#endif

				bool changes = false;
				for(auto itr = n_out.cbegin(); !changes && (itr != n_out.cend()); ++itr)
				{
					point ip = *itr + p;

					// note: async(2) means that active cells can be activated or not
					changes = changes || ( sim_rect.is_inside(ip) &&
						(cur_res_grid[*itr] != (*old_grid)[ip]) && async(2));
					/*if(changes)
					{
						std::cerr << "neq:" << ip << std::endl;
						std::cerr << cur_res_grid[*itr] << " <-> " << (*old_grid)[ip] << std::endl;
					}*/
				}
				if(changes)
				{
					//std::cerr << "FOUND ACTIVE CELL: " << p << std::endl;
					cells_to_check.insert(p);
				}
			}
		};

//		new_grid->reset(std::numeric_limits<int>::min());
		
		// variable cells this round are either neighbours of
		// recently changed cells...
		for(const point& ap : new_changed_cells)
		for(const point& np : n_dep)
		 add_cell_if_variable_and_async(ap + np);
		new_changed_cells.resize(0); // will not affect capacity!

		// ... or neighbours of cells that 
		for(const point& p : cells_not_token)
		 add_cell_if_variable_and_async(p);
		cells_not_token.clear();

	//	std::cerr << "NG:" << std::endl << (*new_grid) << std::endl;
		
		// shuffle the order of active cells
		std::copy(cells_to_check.begin(), cells_to_check.end(), std::back_inserter(change_order));

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(change_order.begin(), change_order.end(), g);

		_grid[2].reset(0); // TODO: reset all?

		std::set<point> final_dec;

		// find out which cells can be token
		// (=> final dec, reserved on _grid[2])
		// and which can not (=> cells not token)
		// TODO: the log factor would be avoidable...
		for(point& cp : change_order)
		{
			const auto point_avail = [&](const point& p){
			//	std::cerr << p << ": " << _grid[2][p] << std::endl;
				return !_grid[2][p]; };
			if(n_out.for_each_bool(cp, point_avail)) {
				const auto reserve_point = [&](const point& p){ _grid[2][p] = 1; };
				n_out.for_each(cp, reserve_point);
				final_dec.insert(cp);
			}
			else {
				//std::cerr << "rejected: "<< cp <<std::endl;
				cells_not_token.insert(cp);
				cp = point(-1, -1);
			}
		}
		change_order.resize(0);

	//	std::cerr << "reserved:" << _grid[2] << std::endl;

		/*for(const point& p : sim_rect)
		if(final_dec.find(p) == final_dec.end())
		 (*new_grid)[p] = (*old_grid)[p];
		else*/
		for(const point& p : cells_not_token)
		{
			// TODO: duplicate of above call!
		/*	ca_calc.next_state
					(&((*old_grid)[p]),
						p, _grid->internal_dim(),
						&((*new_grid)[p]), _grid->internal_dim());*/
			(*new_grid)[p] = (*old_grid)[p];
			/*n_out.for_each(n_out.center(), [&](const point& np){
				(*new_grid)[p + np] = (*old_grid)[p + np];
			});*/
		}

		for(const point& p : final_dec)
		{
			// TODO: duplicate of above call!
		/*	ca_calc.next_state
					(&((*old_grid)[p]),
						p, _grid->internal_dim(),
						&((*new_grid)[p]), _grid->internal_dim());*/
			n_out.for_each(n_out.center(), [&](const point& np){
				(*new_grid)[p + np] = tmp_grid[p][np];
			});
		}

		std::move(final_dec.begin(), final_dec.end(), std::back_inserter(new_changed_cells));

	//	std::cerr << "NOW:" <<  std::endl;
	//	std::cerr << *old_grid;
	//	std::cerr << *new_grid;

		++round;
	}

	// TODO: move up to virtual class
	//! active cells during a stabilisation
	// TODO: not all of these cells must be active
	const std::vector<point>& active_cells() const { return new_changed_cells; }
	bool has_active_cells() const { return !active_cells().empty(); }

	//! runs the whole ca
	template<class Asynchronicity>
	void _run_once(const Asynchronicity& async = synchronous())
	{
		run_once(_grid->human_dim(), async);
	}


	virtual void run_once(const rect& sim_rect) { _run_once(sim_rect, synchronous()); }
	virtual void run_once(const rect& sim_rect, const default_asynchronicity& async) {
		_run_once(sim_rect, async);
	}
	virtual void run_once(const rect& sim_rect, const synchronous& async) {
		_run_once(sim_rect, async);
	}


	//! runs the whole ca
	//template<class Asynchronicity>
	virtual void run_once() { _run_once(synchronous()); }
	virtual void run_once(const default_asynchronicity& async) {
		_run_once(async);
	}
	virtual void run_once(const synchronous& async) {
		_run_once(async);
	}


	//! returns true iff not all cells are inactive
	bool can_run() const {
		return (new_changed_cells.size() || async); // TODO: async condition is wrong
	}

	stable_t gets_stable() const { return stable_t::unknown; }

	//bool is_stable() const { return false; } // TODO!!!!

	bool stabilize()
	{
		if(gets_stable() == stable_t::never)
		 return false;
		else
		{
			while(can_run()) { run_once(); }
			return true;
		}
	}

	// note: the input functions could be put into a different class
	// however, we currently don't think it would be practical

	void input(const point& p) // TODO: ret value?
	{
		//if(has_)
		// TODO: for now, we assume that the ca is always stable
		(*old_grid)[p] = (*new_grid)[p] =
			ca_input.next_state_old(*new_grid, p);
		/*for(const point np : n_in)
		{
			point cur = p + np;
			if(!grid().point_is_on_border(cur)
				&& ca_calc.is_cell_active(*new_grid, cur))
			 new_changed_cells.push_back(cur);
		}*/
		new_changed_cells.push_back(p);

		initialize_first();
	}

	typename Traits::u_coord_t border_width() const {
		return ca_calc.border_width();
	}
};

#if 0
class configuration_graph_types
{
#if 0
protected:
	struct vertex
	{
		configuration conf;
	};
	struct edge {};

	typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, vertex, edge> graph_t;
	typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
	typedef boost::graph_traits<graph_t>::edge_descriptor edge_t;

	typedef std::map<configuration, vertex_t> map_t;
	typedef std::stack<vertex_t> stack_t;
#endif
};


class scientific_ca_t : public ca_simulator_t, public configuration_graph_types
{
#if 0
	class asynchronicity
	{
		std::vector<bool> value;
	public:
		asynchronicity(std::size_t size) :
			value(size) {}

		void increase() {
			for(const bool& b : value) { (void)b; }
		}
		bool operator()(unsigned i) { return value[i]; }
	};

	// TODO: move down to public function?
	graph_t try_all(const rect& sim_rect) const
	{
		graph_t graph;
		stack_t stack;
		map_t map;

		// start vertex
		vertex_t v = boost::add_vertex(graph);
		graph[v].conf = configuration(std::set<point>(), grid());
		try_children(boost::add_vertex(graph), graph, map, stack, sim_rect);

		return graph;
	}

	void try_children(vertex_t node, graph_t& graph, map_t& map, stack_t& stack, const rect& sim_rect) const
	{
		map_t::const_iterator itr;
		configuration c(sim_rect, grid());
		itr = map.find(c);
		if(itr == map.end())
		{ // vertex has not been initialized yet
			vertex_t v = boost::add_vertex(graph);
		//	(*graph)[v].conf =
			map.insert(std::pair<configuration,
				vertex_t>(c, v));

			// Create an edge conecting those two vertices
			edge_t e; bool b;
			if(!stack.empty()) // root can not have parents
			 boost::tie(e,b) = boost::add_edge(stack.top(), v, graph);

			// get child vector
			std::vector<configuration> next_confs;

			// recurse
			stack.push(v);
			try_children(v, graph, map, stack, sim_rect);
			stack.pop();

			//std::cout << "Workgraph: erasing node " << graph[v].conf << std::endl;
		}
		else
		{
			std::cout << "Adding edge, but not node..." << std::endl;
			std::cout << "  (to: " << (graph[node].conf) << ")" << std::endl;

			boost::add_edge(itr->second, node, graph); // TODO: correct?
		}
	}

public:
	using ca_simulator_t::ca_simulator_t;

	graph_t get_all_configurations(const rect& sim_rect)
	{
		return try_all(sim_rect);
	}
#endif
};
#endif

} }

#endif // CA_H
