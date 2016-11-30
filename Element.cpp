#include "Data.h"


/*      Element     */

//  private:
void Element::_SetType(const char &c)
{
    switch (toupper(c))
    {
        case 'R':
            _type = R;
            break;
        case 'E':
            _type = E;
            break;
        case 'J':
            _type = J;
            break;
        default:
            throw -1;
    }
}

void Element::_SetValue(const double &num)
{
    if (_type == R && num < 0)
        throw -1;

    _value = num;
}

void Element::_SetId(const int &id)
{
    if (id < 0)
        throw -1;

    _id = id;
}

//  public:
/* implementation: */

// constructor
Element::Element(const char &t, const int &id, const double &v) 
	:_next(nullptr), _prev(nullptr), _type(R), _id(-1), _value(0)
{
    _SetType(t);
    _SetId(id);
    _SetValue(v);
}

// Type:
// when source transformation only
void Element::ChangeType(const char &c)
{
    _SetType(c);
}

char Element::GetType()
{
    switch (_type)
    {
        case R:
            return 'R';
        case E:
            return 'E';
        case J:
            return 'J';
        default:
            throw -1;
    }
}  

// Ptr:
Element* Element::GetNext()
{
    return (_next);
}  

void Element::SetNext(Element* n)
{
    _next = n;
}

Element* Element::GetPrev()
{
    return (_prev);
}

void Element::SetPrev(Element* n)
{
    _prev = n;
}

// ID:
int Element::GetId()
{
    return (_id);
}

// needed for transforamtions
void Element::ChangeId(const int &num)
{
    _SetId(num);
}

// Value:
double Element::GetValue()
{
    return (_value);
}

// for transformations
void Element::ChangeValue(const int &num)
{
    _SetValue(num);
}