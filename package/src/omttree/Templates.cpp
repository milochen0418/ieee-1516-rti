// ./develope/includeomttree/Templates.cpp
#ifndef TEMPLATES_CPP
#define TEMPLATES_CPP

#include "OHT.cpp"
#include "IHT.cpp"
#include "UniqueStringCollection.cpp"
#include "String.h"

using namespace omt;
class LRC_tree_node;
class LRC_ihtnode;
class LRC_ohtnode;
template class OHT<LRC_tree_node>;
template class OHT<LRC_ohtnode>;

template class OHT<int>;
template class OHT<String>;
template class OHT<double>;
template class OHT<float>;
template class OHT<char>;
template class OHT<void>;
template class OHT<bool>;
template class IHT<int>;

template class IHT<LRC_ihtnode>;
template class IHT<String>;
template class IHT<double>;
template class IHT<float>;
template class IHT<char>;
template class IHT<void>;
template class IHT<bool>;

template class UniqueStringCollection<int>;
template class UniqueStringCollection<String>;
template class UniqueStringCollection<double>;
template class UniqueStringCollection<float>;
template class UniqueStringCollection<char>;
template class UniqueStringCollection<void>;
template class UniqueStringCollection<bool>;
#endif	//TEMPLATES_CPP
