/*     
        Circuit::_List
     my_list to store elements while reading
     used for issue tracking (error handling during reading)
*/
#include "Data.h"

//      private:

// checks for errors and adds element if valid
// throws DUPLICATE_ELEMENT, DUPLICATE_WITH_DIFF_VALUES, SAME_POLARITY
void Circuit::_List::_Check_then_add(Element* e, Node* node)
{
    /*pseudo-code:
        loop my_list in reverse:
            if found:
                if tuple has both nodes:
                    throw DUPLICATE_ELEMENT

                if thisNode is foundNode:
                    throw DUPLICATE_ELEMENT
                
                if (resistances) and (different in value):
                    throw DUPLICATE_WITH_DIFF_VALUES

                if e or j:
                    if same value:
                        throw SAME_POLARITY
                    
                    if given.value != - found.value:
                        throw DUPLICATE_WITH_DIFF_VALUES
            else:
                make new tuple
                add it
    */
    Element *e_in_list;
    Node *foundNode1, *foundNode2;

    for (auto itr = my_list.rbegin(); itr != my_list.rend(); itr++)
    {
        _Parse_ElementTuple_pointers(*itr, e_in_list, foundNode1, foundNode2);

        if (Element::IsSame(e, e_in_list))
        {
            _Check_errors_between_two_nodes(e, e_in_list, foundNode1, foundNode2, node);

            // element is valid, add its node to tuple
            get<2>(*itr) = node;
            return;
        }
    }

    // tuple is not found, lets make new tuple
    ElementTuple tpl(e, node, nullptr);
    my_list.push_back(tpl);
}

// see _Check_then_add()
void Circuit::_List::_Check_errors_between_two_nodes(Element* e, Element* e_in_list, Node* foundNode1, Node* foundNode2, Node* node)
{
    // check errors
    // tuple is full -> duplicate
    if (foundNode1 && foundNode2)
        throw DUPLICATE_ELEMENT;

    // they are in same node -> duplicate
    if (node == foundNode1)
        throw DUPLICATE_ELEMENT;

    if (e->GetType() == 'R') 
    {
        // different in value
        if (e->GetValue()   !=   e_in_list->GetValue())
            throw DUPLICATE_WITH_DIFF_VALUES;
    }
    else
    {
        // same value 
        if (e->GetValue()      ==       e_in_list->GetValue())
            throw SAME_POLARITY;

        // not given == - found
        if (e->GetValue()      !=       - e_in_list->GetValue())
            throw DUPLICATE_WITH_DIFF_VALUES;
    }
}


bool Circuit::_List::_Remove_invalid_voltage_source(tpl_itr &itr, tpl_itr &itr2)
{
    if (_Is_Parallel(*itr, *itr2))
    {
        // fill those
        Element *e, *e2;
        Node *e_term_1, *e_term_2, *e2_term_1, *e2_term_2;
        _Parse_ElementTuple_pointers(*itr, e, e_term_1, e_term_2);
        _Parse_ElementTuple_pointers(*itr2, e2, e2_term_1, e2_term_2);

        // if they are not equall
        // remove both 
        if (e->GetValue()   !=   e2->GetValue())
        {
            int id2 = e2->GetId();

            // tell user
            HandleError(PARALLEL_DIFF_VOLTAGES);

            // remove from term_1
            e_term_1->Remove('E', id2);

            // remove from term_2
            e_term_2->Remove('E', id2);

            // remove tuple from my_list
            my_list.erase(itr2++);
            itr2--;//solves some issues
        }

        // want to remove the other node 
        return true;
    }    

    return false;
}

// BUG: removes every series 
bool Circuit::_List::_Remove_invalid_current_source(tpl_itr &itr, tpl_itr &itr2)
{
    if (_Is_Series(*itr, *itr2))
    {
        // fill those
        Element *e, *e2;
        Node *e_term_1, *e_term_2, *e2_term_1, *e2_term_2;
        _Parse_ElementTuple_pointers(*itr, e, e_term_1, e_term_2);
        _Parse_ElementTuple_pointers(*itr2, e2, e2_term_1, e2_term_2);

        // not equall
        // remove both
        if (e->GetValue()  !=  e2->GetValue())
        {
            int id2 = e2->GetId();

            // tell user
            HandleError(SERIES_DIFF_CURRENTS);

            // remove e2
            e2_term_1->Remove('J', id2);
            e2_term_2->Remove('J', id2);
            
            // remove tuples from my_list
            itr2 = my_list.erase(itr2);
            itr2--;//solves some issues
        }

        // want to remove the other one
        return true;
    }

    return false;
}

void Circuit::_List::_Parse_ElementTuple_pointers(ElementTuple& tpl, Element*& e, Node*& term1, Node*& term2)
{
    ElementTuple& e_tuple = tpl;

    // element
    e = get<0>(e_tuple);

    // terminals in order
    term1 = get<1>(e_tuple);
    term2 = get<2>(e_tuple);
}

void Circuit::_List::_Print_Tuple_list()
{
    Element *e, *e2;
    Node *e_term_1, *e_term_2;

    for (auto itr = my_list.begin(); itr != my_list.end(); itr = next(itr))
    {
        _Parse_ElementTuple_pointers(*itr, e, e_term_1, e_term_2);

        cout << "Element " << e->GetType() << e->GetId()
            << " terminals ";
            
        if (e_term_1)
            cout << e_term_1->GetId();

        if (e_term_2)
            cout << " and " << e_term_2->GetId() << '\n';
    }
}

bool Circuit::_List::_Is_Parallel(const ElementTuple& first, const ElementTuple& second)
{
    // get terminals from the tuple
    Node *e_term_1 = get<1>(first), 
    *e_term_2 = get<2>(first), 

    *e2_term_1 = get<1>(second), 
    *e2_term_2 = get<2>(second);

    return ((e_term_1 == e2_term_1       &&       e_term_2 == e2_term_2) || 
            (e_term_1 == e2_term_2       &&       e_term_2 == e2_term_1));
}

bool Circuit::_List::_Is_Series(const ElementTuple& first, const ElementTuple& second)
{
    // get terminals from the tuple, store them at pointers
    Node 
    *e_term_1 = get<1>(first), 
    *e_term_2 = get<2>(first), 

    *e2_term_1 = get<1>(second), 
    *e2_term_2 = get<2>(second);

    Node* e_terminals[] = {e_term_1, e_term_2};

    /*pseudo-code
        to be series:
            1- first & second must have one common node, not two or zero
            2- that node should have only two elements, the first and second
    */

    // get shared node
    Node* common = nullptr;

    for (const auto terminal: e_terminals)
    {
        if (terminal == e2_term_1 || terminal == e2_term_2)
        {
            if (common) // we have two common then
                return false;
            else
                common = e_term_2;
        }
    }
    
    // common should have only two elements
    return (common && common->GetNumOfElements() == 2);
}

//      public:

// adds address of element in my_list with its corresponding nodes
void Circuit::_List::Add(Element* e, Node* node)
{
    try
    {
        _Check_then_add(e, node);
    }
    catch (const error &err)
    {
        HandleError(err);
        delete e;
        throw err;
    }
}

void Circuit::_List::Pop_back()
{
    my_list.pop_back();
}

// remove lonely elements/nodes
// lonely node: has one element
// lonely element: second terminal is null
void Circuit::_List::Remove_lonelys()
{
    for (auto itr = my_list.begin(); itr != my_list.end(); itr++)
    {
        // parse tuple, *itr is an ElementTuple
        Element* &e = get<0>(*itr);
        Node* terminals[] = {get<1>(*itr), get<2>(*itr)};
        int id = e->GetId();
        char type = e->GetType();
        

        // if lonely element, 
        // remove e from n1, make e null as a sign
        if (!terminals[1])
        {
            HandleError(LONELY_ELEMENT);
            terminals[0]->Remove(e);
            e = nullptr;
        }

        // lonely nodes
        // remove element from other node, maybe that other node is not lonely
        for (int i = 0; i < 2; i++)
        {
            Node* &term = terminals[i];
            Node* &other_term = terminals[(i + 1) % 2];

            // ignore unfound terminals
            if (!term)
                continue;
            
            // node found, see if lonely
            if (term->GetNumOfElements() == 1)
            {
                cerr << HANDLE_NODE_WITH_ONE_ELEM;//TODO: make better

                // remove element from other node if found
                if (other_term)
                    other_term->Remove(type, id);

                // remove the whole node
                term->_Remove_me_from_circ();
                e = nullptr;
                term = nullptr;
            }
        }

        // removing e is a signal that this tuple is no more valid
        if (!e)
            my_list.erase(itr++);
    }
} 

// clears the my_list from data
void Circuit::_List::Clear()
{
    my_list.clear();
}

void Circuit::_List::Remove_invalid_sources()
{
    /*pseudo-code:
        get data
            make my_list for current
            --    --  for voltage
            file them with tuples

        track issues
            v: terminals cant be duplic with different voltage value
            c: one terminal cant be duplic in any with different curr value

        O(n^2)
    */
    
    Element *e, *e2;
    char element_type = '\0';

    // traverse through my_list looking for sources
    for (auto itr = my_list.begin(); itr != my_list.end();) 
    {
        e = get<0>(*itr);
        // detect type of this src and store it here
        element_type = e->GetType();
        // ignore it if resistance
        if (element_type == 'R')
		{
			itr++;
			continue;
		}

		bool found_invalid = false;
        // traverse through rest of my_list
        for (auto itr2 = next(itr); itr2 != my_list.end(); itr2++)
        {
            e2 = get<0>(*itr2);

            // e must be same type as e2
            if (e2->GetType() != element_type)
                continue;

            // see what type is it, and call proper function
            switch (element_type)
            {
                case 'E':
                    found_invalid = _Remove_invalid_voltage_source(itr, itr2);
                    break;
                case 'J':
                    found_invalid = _Remove_invalid_current_source(itr, itr2);
                    break;
                default:
                    assert(FOR_DEBUGGING);
            }
        }

        // remove this 
		if (found_invalid)
        {
            Node *n1 = get<1>(*itr), *n2 = get<2>(*itr);
            int id = e->GetId();

            n1->Remove(element_type ,id);
            n2->Remove(element_type ,id);

            itr = my_list.erase(itr);
        }
        // go to next one
        else
        {
            itr++;
        }
    }
}
