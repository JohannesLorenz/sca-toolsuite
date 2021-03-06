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

#include <cstring>
#include <climits>

#include "simulate.h"
#include "general.h"
#include "io.h"
#include "ca.h"
#include "ca_eqs.h"
#include "ca_table.h"

using namespace sca;

// TODO: own sim type class, inherit
class MyProgram : public Program, sim::ulator
{
	exit_t main()
	{
		const char* equation = "v";
		bool async = false;
		int num_steps = INT_MAX;
		unsigned seed = sca_random::find_good_seed();
		sim_type sim = sim_type::end;

		switch(argc)
		{
			case 6:
				seed = atoi(argv[5]);
			case 5:
				assert_usage(!strcmp(argv[4],"async")
					|| !strcmp(argv[4],"sync"));
				async = (argv[4][0] == 'a');
			case 4:
				num_steps = atoi(argv[3]);
			case 3:
				sim = type_by_str(argv[2]);
				if(sim == sim_type::undefined)
				 exit_usage();
			case 2:
				equation = argv[1];
				break;
			case 1:
			default:
				exit_usage();
		}

		exit_t result;

		// choose ca type
		if(!strncmp(equation, "table:", 6))
		{
			std::ifstream ifs(equation + 6);
			ca::simulator_t<ca::table_t, def_coord_traits,
				def_cell_traits> simulator(ifs);
			result = func(simulator, sim, num_steps, async, seed);
		}
		else
		{
			ca::simulator_t<ca::eqsolver_t, def_coord_traits,
				def_cell_traits> simulator(equation, async);
			result = func(simulator, sim, num_steps, async, seed);
		}

		return result;
	}

	template<class CaType>
	exit_t func(ca::simulator_t<CaType,  def_coord_traits, def_cell_traits>& simulator,
		const sim_type& sim,
		const int& num_steps,
		const bool& async,
		unsigned seed)
	{
		using ca_sim_t = ca::simulator_t<CaType,  def_coord_traits, def_cell_traits>;

		//FILE* const in_fp = stdin;
		//FILE* const out_fp = stdout;
		std::istream& in_fp = std::cin;
		std::ostream& out_fp = std::cout;

		sca_random::set_seed(seed);

		switch(sim)
		{
			case sim_type::role:
				break;
			case sim_type::anim:
				/*os_clear();*/ break;
			case sim_type::more:
				exit("Sorry, `more' is not supported yet.");
			default:
				break;
		}

		in_fp >> simulator.grid();

#if 0
		(void)in_fp;
		grid_t tmp_grid(std::cin, 0); // TODO: in_fp

		cell_t min=INT_MAX, max=INT_MIN;
		for(const cell_t c: tmp_grid)
		{
			min = std::min(min, c);
			max = std::max(max, c);
		}
		assert(min >= 0);
		assert(min <= max);
		ca::ca_simulator_t simulator(equation, max, async);
		simulator.grid() = tmp_grid;
#endif
		simulator.finalize();

		for(int round = 0; (round < num_steps) && simulator.can_run(); ++round)
		{
			if(sim != sim_type::end)
			{
				if(sim == sim_type::anim)
				 os_clear();
				out_fp << simulator.grid() << std::endl;
				switch(sim)
				{
					case sim_type::anim:
						os_sleep(1);
						break;
					case sim_type::more:
						while(getchar()!='\n') ; // TODO: use in_fp
						break;
					default: break;
				}
			}

			// TODO: why is the param necessary?
			if(async)
			 simulator.run_once(typename ca_sim_t::default_asynchronicity());
			else
			 simulator.run_once(typename ca_sim_t::synchronous());
		}

		if(sim == sim_type::anim)
		 os_clear();
		out_fp << simulator.grid();
		if(sim == sim_type::anim) {
			out_fp << std::endl;
			os_sleep(1);
		}

		return exit_t::success;
	}
};

int main(int argc, char** argv)
{
	HelpStruct help;
	help.syntax = "ca/ca <equation> "
		"[<sim_type> [<rounds> [sync|async [seed]]]]";
	help.description = "Runs a cellular automaton (ca).";
	help.input = "start configuration of the ca";
	help.output = "configuration after the simulation";
	help.add_param("equation", "specifies the equation which determines the ca");
	help.add_param("rounds", "number of rounds to simulate; if not given, simulates until stable");

	MyProgram p;
	return p.run(argc, argv, &help);
}

