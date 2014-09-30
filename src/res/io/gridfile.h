/*************************************************************************/
/* sca toolsuite - a toolsuite to simulate cellular automata.            */
/* Copyright (C) 2011-2014                                               */
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

#ifndef GRIDFILE_H
#define GRIDFILE_H

#include <string>

#include "secfile.h"
#include "grid.h"
#include "ca_basics.h"
#include "ca_eqs.h"

namespace sca { namespace io {

class color_t
{
public:
	char r = 0, g = 0, b = 0, a = 255;
	color_t(const char* str);
};

// TODO: -> eqsolver?
class color_formula_t
{
	ca::eqsolver_t eqs;

public:
	color_formula_t(const char* formula) : eqs(formula, 0) {}

	int32_t operator()(const grid_t& g, point p) const {
		return eqs.calculate_next_state_old<
			def_coord_traits, def_cell_traits
			>(&g[p], p, g.human_dim());
	}
};

class path_node
{
	// TODO: cpp?
	inline static const char* next_word(const char*& str) {
		while(*(++str) != ' ') ;
		return ++str;
	}
public:
	//int grid_id;
	std::string description;

	enum class mark_type
	{
		active,
		formula
	};

	class markup
	{
	public:
		color_t color;
		mark_type tp;
		std::string formula;
		markup(const char* str);
	};

	class arrow
	{
	public:
		point p1, p2;
		// TODO: const ctor
		arrow(const char* str);
	};

	std::vector<markup> markup_list;
	std::vector<arrow> arrow_list;

	bool parse(secfile_t& inf);

	void dump(std::ostream& stream) const { (void)stream; /*TODO*/ }
};

template<> // TODO: abstract case of path_node? enable_if?
class leaf_template_t<path_node> : public leaf_base_t
{
	path_node t;
public:
	void parse(secfile_t& inf) { t.parse(inf); }
	void dump(std::ostream& stream) const { t.dump(stream); }
	const path_node& value() { return t; }
};

class scene_grids_t : public supersection_t
{
public:
	scene_grids_t() : supersection_t(type_t::multi) {
		init_factory<leaf_template_t<grid_t>>();
	}
};

class scene_path_t : public supersection_t
{
public:
	scene_path_t() : supersection_t(type_t::multi) {
		init_factory<leaf_template_t<path_node>>();
	}
};


class gridfile_t : public supersection_t // TODO: public?
{
public:
	gridfile_t() : supersection_t(type_t::batch)
	{
		init_leaf<leaf_template_t<std::string>>("rgb32");
		init_leaf<leaf_template_t<std::string>>("name"); // TODO: init_leafs ;)
		init_leaf<leaf_template_t<std::string>>("description");
		init_leaf<leaf_template_t<ca::n_t>>("n");
		init_subsection<scene_grids_t>("grids");

		init_factory<scene_path_t>();
		set_batch_str("path");
	}

/*	void get_tv()
	{


		tv_ctor cons(n);
		for(const std::vector<path_node>& v : paths)
		{
			using itr_t = std::vector<path_node>::const_iterator;
			itr_t tar = v.begin();
			itr_t src = tar++;
			for(; tar != v.end(); ++src, ++tar)
			{
				cons.add_eq(grids[src->grid_id], grids[tar->grid_id]);
			}
		}

		tv = std::move(trans_vector_t(std::move(cons)));
	} */
};

}}

#endif // GRIDFILE_H
