#ifndef __RTI_DEBUG_H
#define __RTI_DEBUG_H

#include "RTI_Typedefs.h"
#include "PubSubTable.h"

void Display_AllCDC(OHTTable *ohttable);
void Display_AllCDC(IHTTable *ihttable);
void Display_AllSubscribe(OHTTable *ohttable);
void Display_AllSubscribe(IHTTable *ihttable);
void Debug_AttributeList(int classhandle, RTI_AttributeHandleSet const & attributeList);
void Debug_PublishedAttributeList(OHTTable *ohttable, int root);
void Debug_SubscribedAttributeList(OHTTable *ohttable, int root);
void Debug_UpdateAttributeList(int objectinstance, RTI_AttributeHandleValueMap const & AttributeValues);
void Debug_SendInteractionList(int classhandle, RTI_ParameterHandleValueMap const & ParameterValues);

#endif

/*
 * vim: ts=4 sts=4 sw=4
 */
