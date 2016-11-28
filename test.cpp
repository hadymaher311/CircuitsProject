#include "Solver.h"
#include "Data.h"

void mat()
{
    Solver s(2,2);
    s.A << 1 ,5, -1, 2;
    s.B << 3, 5;
    s.Print();
    s.Solve();
    cout << s.X;

}

void element1()
{
    Element* n = new Element("t", 55);
    delete n;
}

int main()
{
    //mat();
    element1();
}