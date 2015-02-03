/*************************************************************************/
/* sca toolsuite - a toolsuite to simulate cellular automata.            */
/* Copyright (C) 2011-2015                                               */
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

#include "io/gridfile.h"
#include "general.h"
#include "ca_basics.h"
#include "ca_convert.h"

class MyProgram : public Program
{
	exit_t main()
	{
		/*
		 * args
		 */

		switch(argc)
		{
			case 2:
			//	tbl_file = argv[1];
			// TODO: output without changes?
				assert_usage(!strcmp(argv[1], "tex"));
				break;
			case 1: break;
			default:
				exit_usage();
		}

		/*
		 * parsing
		 */

		sca::io::secfile_t inf;
		sca::io::gridfile_t scene;
		try {
			scene.parse(inf);
		} catch(sca::io::secfile_t::error_t ife) {
			std::cout << "infile line " << ife.line << ": "	 << ife.msg << std::endl;
		}
		std::cout << scene;

		return exit_t::success;
	}
};

int main(int argc, char** argv)
{
	HelpStruct help;
	help.syntax = "ca/scene [<out format>]"
		"";
	help.description = "Converts a scene into a document file."
		"";
	help.input = "Input grid in a special format.";
	help.output = "The ca document";
	help.add_param("<out format>", "output format. currently only: tex (=default)");

	MyProgram p;
	return p.run(argc, argv, &help);
}

