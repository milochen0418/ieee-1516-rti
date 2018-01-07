#include "PubSubTable.h"
extern "C"
{
#include "debug.h"
}

void initialize_ohttable(OHT<int> *oht, OHTTable *ohttable)
{
	int i, j, num_class, num_attr;
	int indent = 0;
	
	num_class = oht->getClassNodeCount();
	ohttable->Asubscribe = new bool*[num_class + 1];
	ohttable->Apublish = new bool*[num_class + 1];
	ohttable->OCsubscribe = new bool[num_class + 1];
	ohttable->OCpublish = new bool[num_class + 1];
	ohttable->CDC = new uint32_t[num_class + 1];

	debug(DEBUG_OMT, "%s: OHT lists:\n", __func__);
	for(i=1; i<=num_class; i++)
	{
		num_attr = oht->getNumberOfAttribute(i);
		ohttable->Asubscribe[i] = new bool[num_attr + 1];
		ohttable->Apublish[i] = new bool[num_attr + 1];
		for(j=1; j<=num_attr; j++)
		{
			 ohttable->Asubscribe[i][j] = false;
			 ohttable->Apublish[i][j] = false;
		}

		ohttable->OCsubscribe[i] = false;
		ohttable->OCpublish[i] = false;
		ohttable->CDC[i] = false;
		
		debug(DEBUG_OMT, "    OC=%2lu  tail=%2lu  #attr=%2lu  name= %s\n",
				i, oht->getClassTailValue(i), oht->getNumberOfAttribute(i), 
				oht->getObjectClassName(i));
	}

	debug(DEBUG_OMT, "\n");
	debug(DEBUG_OMT, "    1(%s: %lu)\n", oht->getObjectClassName(1), oht->getNumberOfAttribute(1));
	for (i=2; i<= num_class; i++)
	{
		debug(DEBUG_OMT, "    ");
		
		indent++;
		for (j=0;j<indent;j++)
		{
			if (j == indent-1)
				debug_p(DEBUG_OMT, "|___");
			else
				debug_p(DEBUG_OMT, "    ");
		}
		debug_p(DEBUG_OMT, "%d(%s: %lu)\n", i, oht->getObjectClassName(i), oht->getNumberOfAttribute(i));
		
		if (i == (int)oht->getClassTailValue(i))
			indent--;
		if (i == (int)oht->getClassTailValue(oht->getClassParentNode(i)))
		{
			indent--;
			debug(DEBUG_OMT, "\n");
		}
	}
}

void finalize_ohttable(OHTTable *ohttable)
{
	delete [] ohttable->Asubscribe;
	delete [] ohttable->Apublish;
	delete [] ohttable->OCsubscribe;
	delete [] ohttable->OCpublish;
	delete [] ohttable->CDC;
}


void initilaize_ihttable(IHT<int> *iht, IHTTable *ihttable)
{
	int i, j, num_class;
	int indent = 0;
	
	num_class = iht->getClassNodeCount();
	ihttable->ICsubscribe = new bool[num_class + 1];
	ihttable->ICpublish = new bool[num_class + 1];
	ihttable->CDC = new uint32_t[num_class + 1];
	ihttable->M = new uint32_t[num_class + 1];
	
	debug(DEBUG_OMT, "%s: IHT lists:\n", __func__);
	for(i=1; i<=num_class; i++)
	{
	  ihttable->ICsubscribe[i] = false;
	  ihttable->ICpublish[i] = false;
	  ihttable->CDC[i] = false;
	  ihttable->M[i] = iht->getNumberOfParameter(i);
	  
	  debug(DEBUG_OMT, "    IC=%2lu  tail=%2lu  #param=%2lu  name= %s\n",
			  i, iht->getClassTailValue(i), iht->getNumberOfParameter(i), 
			  iht->getInteractionClassName(i));
	}

	debug(DEBUG_OMT, "\n");
	debug(DEBUG_OMT, "    1(%s: %lu)\n", iht->getInteractionClassName(1), iht->getNumberOfParameter(1));
	for (i=2; i<= num_class; i++)
	{
		debug(DEBUG_OMT, "    ");
		
		indent++;
		for (j=0;j<indent;j++)
		{
			if (j == indent-1)
				debug_p(DEBUG_OMT, "|___");
			else
				debug_p(DEBUG_OMT, "    ");
		}
		debug_p(DEBUG_OMT, "%d(%s: %lu)\n", i, iht->getInteractionClassName(i), iht->getNumberOfParameter(i));
		
		if (i == (int)iht->getClassTailValue(i))
			indent--;
		if (i == (int)iht->getClassTailValue(iht->getClassParentNode(i)))
		{
			indent--;
			debug(DEBUG_OMT, "\n");
		}
	}

}

void finalize_ihttable(IHTTable *ihttable)
{
	delete [] ihttable->ICsubscribe;
	delete [] ihttable->ICpublish;
	delete [] ihttable->CDC;
	delete [] ihttable->M;
}

/*
 * vim: ts=4 sts=4 sw=4
 */
