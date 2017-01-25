#include "Solving.h"

// remove all Voltage sources and current sources
void Disable_Sources(Circuit* c);

// calculate voltage between two nodes
int Voltage(Node* n1, Node* n2);

// calculate amper between two nodes
int Ampere(Node* n1, Node* n2, Element* e);

// calculate Power between two nodes
int Power(Element* e, Circuit* c);

// find the two nodes that contains that element 
void Get_2_Nodes(Element* e, Node* &n1, Node* &n2, Circuit* c);


// remove all Voltage sources and current sources
void Disable_Sources(Circuit* c)
{
	Node* n = c->GetFirstNode();
	while(n != NULL)
	{
		Element* e = n->GetFirstElement();
		while(e != NULL)
		{
			Element* temp = e->GetNext();
			//TODO E
			if(e->GetType() == 'E' || e->GetType()=='J')
				n->Remove(e);     //to remove Voltage sources and current sources
			
			e = temp;
		}

		n = n->GetNext();
	}
}

// calculate voltage between two nodes
int Voltage(Node* n1, Node* n2)
{
	return(n1->GetVolt()-n2->GetVolt());     //v1-v2
}

// calculate amper between two nodes
int Ampere(Node* n1, Node* n2, Element* e)
{
	// CHECK THAT E IS A RESISTANCE
	return ((n1->GetVolt() - n2->GetVolt()) / e->GetValue());     //(v1-v2)/r 
}

// calculate Power between two nodes
int Power(Element* e, Circuit* c)
{
	Node* n=c->GetFirstNode();
	Node* n1=c->GetFirstNode();
	Node* n2=c->GetFirstNode();
	Element* e_temp=n->GetFirstElement();
	int id=e->GetId();
	int i=0;
	int I;
    Get_2_Nodes(e,n1,n2,c);
		switch (id)
	{
	case'V':
		{
			if(!n1->IsEssential())
			{
				if(n1->GetFirstElement()->GetId() != id)
				{
					Get_2_Nodes(n1->GetFirstElement(),n1,n2,c);

				}
				else
				{
					e_temp=n1->GetFirstElement()->GetNext();
					Get_2_Nodes(e_temp,n1,n2,c);
				}
				I= ( (n1->GetVolt()-n2->GetVolt() ) /e_temp->GetValue());
				return (I*e->GetValue());
			}
			else
			{
				if(n2->GetFirstElement()->GetId() != e->GetId())
				{
					Get_2_Nodes(n2->GetFirstElement(),n1,n2,c);

				}
				else
				{
					e_temp = n2->GetFirstElement()->GetNext();
					Get_2_Nodes(e_temp,n1,n2,c);
				}

			}
			
			return((n1->GetVolt()-n2->GetVolt())*e->GetValue()*e->GetValue());   

			}
	case'I':
		{
			//volt n1 - volt n2= v12 \\P = v12 * I
			return((n1->GetVolt()-n2->GetVolt())*e->GetValue());
		}

	default: //R
		//volt n1 - volt n2= v12 \\ P = [v12 *V12]/ R
		return(((n1->GetVolt()-n2->GetVolt())*(n1->GetVolt()-n2->GetVolt()))/e->GetValue());
		break;
		}


}

// find the two nodes that contains that element 
void Get_2_Nodes(Element* e, Node* &n1, Node* &n2, Circuit* c)
{
	Node* n = c->GetFirstNode();
	int i=0;
	int j=0;
	int id = e->GetId();
	char type = e->GetType();

	while(i<=1)
	{
		Element* e_temp = n->GetFirstElement();
		while(e_temp != NULL && j == 0)
		{
			if(e_temp->GetId() == id && e_temp->GetType() == type && i <= 0)
			{
				n1=n;
				j=1;
			}
			else if(e_temp->GetId() == id && e_temp->GetType() == type)
			{
				n2=n;
				j=1;
			} 
			else
				e_temp = e_temp->GetNext();
		}

		if (e_temp == NULL || j == 0)
		{
			n = n->GetNext();
		}
		else
		{
			j=0;
			n = n->GetNext();
			i++;
		}
	}
}