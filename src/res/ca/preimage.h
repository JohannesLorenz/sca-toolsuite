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

#ifndef PREIMAGE_H
#define PREIMAGE_H

#include "ca.h"
#include <iostream>
#include <cmath>

// TODO: cpp file

namespace sca { namespace ca {

// TODO: stackoverflow
int my_pow(int x, int p)
{
	if (p == 0) return 1;
	if (p == 1) return x;

	int tmp = my_pow(x, p/2);
	if (p%2 == 0) return tmp * tmp;
	else return x * tmp * tmp;
}

//! just for debugging. reentrancy not dangered
point max_point = point (0, 0);

//! fills in possible grids for preimages from top-left to bottom right
template<class Solver, class Traits, class CellTraits>
void _dump_preimages(const _calculator_t<Solver, Traits, CellTraits>& ca,
	const _grid_t<Traits, CellTraits>& input,
	std::size_t num_states,
	_grid_t<Traits, CellTraits>& constraints,
	const std::ostream& ofs = std::cout,
	_point<Traits> this_point = _point<Traits>(1, 1))
{

	using point = _point<Traits>;
	using grid_t = _grid_t<Traits, CellTraits>;
	grid_t src_grid(ca.n_in().dim(), 0);
	const point c = ca.n_in().center();

//	std::cerr << this_point << std::endl;

	if(this_point.y == static_cast<typename Traits::coord_t>(input.dy() - 1))
	{
		// recursion is finished
		std::cout << constraints;
	}
	else
	{
		if(this_point > max_point)
		{
			max_point = this_point;
			std::cerr << "NEW MAX: " << this_point << std::endl;
		}

		// fill src_grid
		std::vector<point> variables;

		for(const point& np : ca.n_in().get_rect())
		{
			if(constraints[this_point + np] < 0)
			 variables.push_back(np);
			else
			 src_grid[c + np] = constraints[this_point + np];
		}

		point next_point = this_point + point(1, 0);
		if(next_point.x == static_cast<typename Traits::coord_t>(input.dx() - 1))
		{
			next_point.x = 1;
			++next_point.y;
		}

		std::size_t remains = variables.size();
		std::size_t mask_max = my_pow(num_states, remains);

		for(std::size_t mask = 0; mask < mask_max; ++mask)
		{
			std::size_t mask_cp = mask;
			for(std::size_t count = 0; count < remains; ++count)
			{
				const point& _p = variables[count];
				constraints[this_point + _p] = src_grid[c + _p] = mask_cp % num_states;
				mask_cp /= num_states;
			}

			bitgrid_t b_res(2 /*TODO!*/, convert<bitgrid_traits>(src_grid.human_dim()), 0, 0, 0);
			ca.next_state(src_grid, c, b_res);
			uint64_t res = b_res[_point<bitgrid_traits>(0, 0)];
			if(static_cast<typename Traits::coord_t>(res) == input[this_point])
			{
				std::cerr << " percentage: " << this_point << ", " << mask << " of " << mask_max << std::endl;
				_dump_preimages(ca, input, num_states, constraints, ofs, next_point);
			}
		}

		for(const point& np : variables)
		 constraints[this_point + np] = -1;

	}
}

template<class Solver, class Traits, class CellTraits>
void dump_preimages(const _calculator_t<Solver, Traits, CellTraits>& ca,
	const _grid_t<Traits, CellTraits> input,
	std::size_t num_states,
	const std::ostream& ofs = std::cout,
	_point<Traits> this_point = _point<Traits>(1, 1))
{
	_grid_t<Traits, CellTraits> constraints(input.human_dim(), 0, -1, 0);
	// draw border - is known not to have changed
	for(const _point<Traits>& p : constraints.points())
	if(p.x == 0
		|| p.x == static_cast<typename Traits::coord_t>(constraints.dx() - 1)
		|| p.y == 0
		|| p.y == static_cast<typename Traits::coord_t>(constraints.dy() - 1))
	 constraints[p] = input[p];

	//std::cerr << constraints << std::endl;

	// start recursion
	_dump_preimages(ca, input, num_states, constraints, ofs, this_point);
}

} }

#endif // PREIMAGE_H
