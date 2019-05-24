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

#include "general.h"
#include "grid.h"
#include "ca/dead_cells.h"
#include "ca_table.h"

using namespace sca;

class MyProgram : public Program
{
	exit_t main()
	{
		const char *tbl_name = nullptr;

		switch(argc)
		{
			case 2: tbl_name = argv[1];
				break;
			case 1:
			default:
				exit_usage();
		}

		std::ifstream in(tbl_name);
		using calc_t =
		ca::_calculator_t<ca::table_t, def_coord_traits,
			def_cell_traits>;
		calc_t ca(in);

		grid_t input(std::cin, ca.border_width());

		grid_t result = input;

		for(const point& p : input.points()) {
			result[p] = ca.is_cell_active(input, p);
		}
		std::cout << result;

		return exit_t::success;
	}
};

int main(int argc, char** argv)
{
	HelpStruct help;
	help.syntax = "ca/active_cells <ca-table-file>"
		"";
	help.description = "Visualizes which cells are active";
	help.add_param("<ca-table-file>", "path to ca in table format");
	help.input = "the input configuration";
	help.output = "boolean grid which is true where the input grid is active";

	MyProgram p;
	return p.run(argc, argv, &help);
}


