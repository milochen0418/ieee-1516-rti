#include <map>
#include <set>
#include "OMT_Parser.h"
#include "OMTDebug.h"
extern "C"
{
#include "debug.h"
}

extern OHT<int> *oht;
extern IHT<int> *iht;

void Display_AllCDC(OHTTable *ohttable)
{
	int i, max;

	debug(DEBUG_OMT, "current OHT CDC: (non-zero CDC only)\n");
	max = (int)oht->getClassNodeCount();
	for(i=1; i<=max; i++)
	{
		if (ohttable->CDC[i] != 0)
			debug(DEBUG_OMT, "    CDC[%2d] = %d\n", i, ohttable->CDC[i]);
	}
}

void Display_AllCDC(IHTTable *ihttable)
{
	int i, max;

	debug(DEBUG_OMT, "current IHT CDC: (non-zero CDC only)\n");
	max = (int)iht->getClassNodeCount();
	for(i=1; i<=max; i++)
	{
		if (ihttable->CDC[i] != 0)
			debug(DEBUG_OMT, "    CDC[%2d] = %d\n", i, ihttable->CDC[i]);
	}
}

void Display_AllSubscribe(OHTTable *ohttable)
{
	int i, max;
	
	debug(DEBUG_OMT, "current OHT subscribing: (non-zero subscription only)\n");
	max = (int)oht->getClassNodeCount();
	for(i=1; i<=max; i++)
	{
		if (ohttable->OCsubscribe[i] != 0)
			debug(DEBUG_OMT, "    OCsubscribe[%2d] = %d\n", i, ohttable->OCsubscribe[i]);
	}
}

void Display_AllSubscribe(IHTTable *ihttable)
{
	int i, max;
	
	debug(DEBUG_OMT, "current IHT subscribing: (non-zero subscription only)\n");
	max = (int)iht->getClassNodeCount();
	for(i=1; i<=max; i++)
	{
		if (ihttable->ICsubscribe[i] != 0)
			debug(DEBUG_OMT, "    OCsubscribe[%2d] = %d\n", i, ihttable->ICsubscribe[i]);
	}
}


void Debug_AttributeList(int classhandle, RTI_AttributeHandleSet const & attributeList)
{
	RTI_AttributeHandleSet::iterator iter;

	debug(DEBUG_OMT, "    object class = %d\n", classhandle);
	debug(DEBUG_OMT, "    attribute = ");
	for(iter=attributeList.begin(); iter!=attributeList.end(); iter++)
	{
		debug_p(DEBUG_OMT, "%ld ", iter->getHandle());
	}
	debug_p(DEBUG_OMT, "\n");
}

void Debug_PublishedAttributeList(OHTTable *ohttable, int root)
{
	int i, flag=0;
	int max = oht->getNumberOfAttribute(root);

	for(i=1; i<=max; i++)
	{
		if(ohttable->Apublish[root][i])
		{
			if (flag == 0)
			{
				debug(DEBUG_OMT, "    object class %d, attribute: ", root);
				flag = 1;
			}
			debug_p(DEBUG_OMT, "%d ",i);
		}
	}
	if (flag == 1)
		debug_p(DEBUG_OMT, "\n");
}

void Debug_SubscribedAttributeList(OHTTable *ohttable, int root)
{
	int i, flag=0;
	int max = oht->getNumberOfAttribute(root);

	for(i=1; i<=max; i++)
	{
		if(ohttable->Asubscribe[root][i])
		{
			if (flag == 0)
			{
				debug(DEBUG_OMT, "    object class %d, attribute: ", root);
				flag = 1;
			}
			debug_p(DEBUG_OMT, "%d ", i);
		}
	}
	if (flag == 1)
		debug_p(DEBUG_OMT, "\n");
}

void Debug_UpdateAttributeList(int objectinstance, RTI_AttributeHandleValueMap const & AttributeValues)
{
	RTI_AttributeHandleValueMap::const_iterator iter;
	
	debug(DEBUG_OMT, "    update object instance %d, attribute: ", objectinstance);
	for(iter=AttributeValues.begin(); iter != AttributeValues.end() ; iter++)
	{
		debug_p(DEBUG_OMT, "%ld ", iter->first.getHandle());
	}
	debug_p(DEBUG_OMT, "\n");
}

void Debug_SendInteractionList(int classhandle, RTI_ParameterHandleValueMap const & ParameterValues)
{
	RTI_ParameterHandleValueMap::const_iterator iter;
	
	debug(DEBUG_OMT, "    send interaction %d, parameter: ", classhandle);
	for(iter=ParameterValues.begin(); iter!=ParameterValues.end(); iter++)
	{
		debug_p(DEBUG_OMT, "%ld ", iter->first.getHandle());
	}
	debug_p(DEBUG_OMT, "\n");
}

/*
 * vim: ts=4 sts=4 sw=4
 */
