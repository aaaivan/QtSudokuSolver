/*
    Copyright (C) 2018  Gregory J. Karanikas.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "dancing_links.h"

namespace dancing_links_GJK
{

/*
 *  an implementation of Donald Knuth's Dancing Links algorithm
 *  https://arxiv.org/pdf/cs/0011047.pdf
 *  https://en.wikipedia.org/wiki/Dancing_Links
 */




void Exact_Cover_Solver(LMatrix& M, std::list<S_Stack>& foundSolutions, const size_t maxSolutionsCount, const bool* abort)
{
    H_Stack history;
    std::vector<size_t> solution;
    solution.reserve(M.number_of_rows());
    DLX(M, solution, history, foundSolutions, maxSolutionsCount, abort);
    solution.shrink_to_fit();
}



void DLX(LMatrix& M, S_Stack& solution, H_Stack& history, std::list<S_Stack>& foundSolutions, const size_t maxSolutionsCount, const bool* abort)
{
    Column *c = choose_column(M);
    // 'M' is empty => solution successfully found
    if( c == NULL ) {
        foundSolutions.push_back(solution);
        return;
    }
    for( MNode *r = c->down(); r != static_cast<MNode*>(c); r = r->down() ) {
        update(M, solution, history, r);
        DLX(M, solution, history, foundSolutions, maxSolutionsCount, abort);
        downdate(M, solution, history);
        if(*abort == true || foundSolutions.size() >= maxSolutionsCount)
        {
            return;
        }
    }
}

/* Given a matrix of linked nodes M, return a pointer to the column with the fewest nodes
 * If there are no columns, return NULL
 */
Column* choose_column(LMatrix& M)
{
    if( M.is_trivial() ) return NULL;

    Column* col = static_cast<Column*>(M.head()->right());
    Column* max_col = col;

    while( col != M.head() )
    {
        if( col->size() < max_col->size() ) {
            max_col = col;
        }
        col = static_cast<Column*>(col->right());
    }
    return max_col;
}

/*
 *
 */
void update(LMatrix& M, S_Stack& solution, H_Stack& history, MNode *r)
{
    solution.push_back(r->data().row_id);

    RC_Stack temp_stack;
    RC_Item temp_item;

    for(MNode * i = r->right(); i != r; i = i->right()) {
        for(MNode *j = i->up(); j != i; j = j->up() ) {
            if(j->data().column_id == j) continue;
            M.remove_row(j);
            temp_item.node = j;
            temp_item.type = RC::row;
            temp_stack.push(temp_item);
        }
        M.remove_column(i);
        temp_item.node = i;
        temp_item.type = RC::column;
        temp_stack.push(temp_item);
    }

    for(MNode *j = r->up(); j != r; j = j->up() ) {
        if(j->data().column_id == j) continue;
        M.remove_row(j);
        temp_item.node = j;
        temp_item.type = RC::row;
        temp_stack.push(temp_item);
    }
    M.remove_column(r);
    temp_item.node = r;
    temp_item.type = RC::column;
    temp_stack.push(temp_item);

    history.push(temp_stack);
}


/*
 * Undoes the operations of 'update'
 */
void downdate(LMatrix& M, S_Stack& solution, H_Stack& history)
{
    if( history.empty() ) {
        return;
    }
    solution.pop_back();
    RC_Stack last = history.top();
    RC_Item it;
    while( !last.empty() ) {
        it = last.top();
        if( it.type == RC::row ) {
            M.restore_row(it.node);
        } else if( it.type == RC::column ) {
            M.restore_column(it.node);
        }
        last.pop();
    }
    history.pop();
}


}
