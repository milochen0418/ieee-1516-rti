#ifndef __MACRO_H
#define __MACRO_H

/* check if federation is initialized or not */
#define THROW_HAS_INITIALIZED() \
{ \
	if (is_initialized == RTI_true) \
	{ \
		char name[100] = {'\0'}; \
		WStrtoStr(name, fullPathNameToTheFDDfile, 99); \
		throw RTI_FederationExecutionAlreadyExists("Federation execution already exists - " + string(name)); \
	} \
}

#define THROW_NOT_INITIALIZED() \
{ \
	if (is_initialized == RTI_false) \
		throw RTI_FederationExecutionDoesNotExist("Federation execution does not exist."); \
}

/* check if federate is joined or not */
#define THROW_CURRENTLY_JOINED() \
{ \
	if (is_joined == RTI_true) \
	{ \
		char name[100] = {'\0'}; \
		WStrtoStr(name, FederationName, 99); \
		throw RTI_FederatesCurrentlyJoined("Federate currently joined federation " + string(name)); \
	} \
}

#define THROW_HAS_JOINED() \
{ \
	if (is_joined == RTI_true) \
	{ \
		char name[100] = {'\0'}; \
		WStrtoStr(name, FederationName, 99); \
		throw RTI_FederateAlreadyExecutionMember("Federate already member of federation " + string(name)); \
	} \
}

#define THROW_NOT_JOINED() \
{ \
    if (is_joined == RTI_false) \
        throw RTI_FederateNotExecutionMember("RTI_FederateNotExecutionMember when Invoke getObjectInstanceHandle()"); \
}

#endif
/*
 * vim: ts=4 sts=4 sw=4
 */
