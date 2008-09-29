#ifndef PHC_SSA
#define PHC_SSA

#include <boost/graph/properties.hpp>
#include <stack>

using namespace boost;

#include "CFG.h"
class Dominance
{
	// Use vertexes so that the Blocks they refer to can be updated without
	// invalidating the dominance information.

	// Forward dominance frontier
	map <vertex_t, list<vertex_t> > df;

	// BB -> blocks dominated by BB
	map <vertex_t, list<vertex_t> > idominated;

	// BB -> BB's dominator
	map <vertex_t, vertex_t> idominator;

	CFG* cfg;


public:
	Dominance (CFG* cfg);

	void dump ();

	// Terms are defined in a comment in SSA.cpp.
	void calculate_forward_dominance ();
	void calculate_reverse_dominance ();
	void calculate_dominance (Graph& graph, vertex_t entry);

	// Dominance on the reverse CFG
	Dominance* reverse_dominance;

private:
	friend class Basic_block;
	BB_list* get_bb_dominance_frontier (Basic_block*);
	void add_to_bb_dominance_frontier (Basic_block* bb, Basic_block* frontier);
	bool is_bb_in_dominance_frontier (Basic_block* bb, Basic_block* frontier);

	Basic_block* get_bb_immediate_dominator (Basic_block*);
	BB_list* get_blocks_dominated_by_bb (Basic_block* bb);
	bool is_bb_dominated_by (Basic_block* bb, Basic_block* potential_dom);

};

// Renaming (Cooper/Torczon, setion 9.3.4).

/* Given a BB, this will perform recursively perform SSA renaming, descending
 * the dominator tree. COUNTER is the version of the next SSA_NAME. VAR_STACKS
 * is the stack of versions used by a named variable. When returning from
 * recursing, the stack is popped to reveal the version used on the previous
 * level. */
class SSA_renaming
{
	CFG* cfg;

	/* Following the example of GCC, we give each SSA_NAME a distinct number.
	 * So instead of x_0, y_0, x_1, y_1, as in textbooks, we use x_0, y_1,
	 * x_2, y_2. This allows us use the version as an index into a bitvector
	 * (which we may or may not do in the future).
	 */
	int counter;
	map<string, stack<int> > var_stacks;

public:
	SSA_renaming (CFG* cfg);

	/*
	 * Public interface
	 */
	void rename_vars (Basic_block* bb);

private:
	/*
	 * Helper functions
	 */
	void push_to_var_stack (MIR::VARIABLE_NAME* var_name, int version);
	int read_var_stack (MIR::VARIABLE_NAME* var_name);
	void pop_var_stack (MIR::VARIABLE_NAME* var_name);

	// Rename the variable into SSA, giving it a version.
	void create_new_ssa_name (MIR::VARIABLE_NAME* var_name);
	void debug_var_stacks ();
};

#endif // PHC_SSA
