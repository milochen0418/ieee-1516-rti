/*
 * Publish and subscribe tables.
 */

#ifndef _OHT_H
#define _OHT_H

#include <map>
#include <set>
#include <stdint.h> 
#include "RTI_SpecificTypedefs.h"
#include "OMT_Parser.h"


/* Definition. P/S state and CDC property for each Object Class of OHT */
struct OHTTable
{
	bool ** Asubscribe;		/* Asubscribe[i][j] = true iff (OC=i,attr=j) have being subscribed */
	bool ** Apublish;		/* Apublish[i][j] = true iff (OC=i,attr=j) have being published */
	bool *  OCsubscribe;	/* OCsubscribe[i] = true iff (OC=i) has being subscribed. */
	bool *  OCpublish;		/* OCpublish[i] = true iff (OC=i) has being published. */
	uint32_t* CDC;			/* CDC[i] == j iff the candicated discovery class of OC=i is OC=j */
};


/*Definition. P/S state and CDC property for for each Interaction Class of IHT */
struct IHTTable
{
	bool *  ICsubscribe;	/* ICsubscribe[i] = true iff (IC=i) has being subscribed. */
	bool *  ICpublish;		/* ICpublish[i] = true iff (IC=i) has being published. */
	uint32_t* CDC;			/* CDC[i] == j iff the candicated discovery class of IC=i is IC=j */
	uint32_t* M;			/* M[i] = j iff the number of paremeter for (IC=i) is j */
	
};

typedef uint32_t Inst_id;

class InstNameOC
{
	public:
		RTI_wstring InstName;
		RTI_ObjectClassHandle OC;
};

typedef struct RTI_ObjectInstanceID
{
	   uint32_t vid;
	   RTI_ObjectInstanceHandle oid;
}ObjectInstanceID;

typedef std::map<RTI_ObjectInstanceHandle,ObjectInstanceID > RTI_ObjectIDMap;

typedef std::pair<RTI_ObjectInstanceHandle, InstNameOC> InstPair;
typedef std::map<RTI_ObjectInstanceHandle, InstNameOC> InstMap;
typedef std::set<RTI_ObjectInstanceHandle> InstSet;
typedef std::set<RTI_wstring> InstNameSet;

void initialize_ohttable(OHT<int> *oht, OHTTable *ohttable);
void initilaize_ihttable(IHT<int> *iht, IHTTable *ihttable);
void finalize_ohttable(OHTTable *ohttable);
void finalize_ihttable(IHTTable *ihttable);

#endif	//_EASY_OHT_H

/*
 * vim: ts=4 sts=4 sw=4
 */
