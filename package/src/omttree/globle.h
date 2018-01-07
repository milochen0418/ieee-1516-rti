#ifndef GROBLE_H
#define GROBLE_H

#ifndef NULL
#define NULL __null
#endif

#define NEWLINE			0x0a
#define CARRIAGERETURN		0x0d
#define XML_STRING		String
#define MAX_ABBRIBUTE_NO 	100
#define MAX_PARAMETER_NO	100
#define MAX_CHILD_NO   		500
#define MAX_NODE_NO 		500

using namespace std;

//////////////////////////////////////////////////////////////
/*      Internal structure for tracking glocation of items
        in the XML file.
*/
struct XmlCursor
{
    XmlCursor()             { Clear(); }
    void Clear()    { row = col = -1; }

    int row;        // 0 based.
    int col;        // 0 based.
};

enum xmlType
{
    type_xmlHeader = 1,
    type_dtdHeader,
    type_commentHeader,
    type_objectModel,
    type_objectCollection,
    type_endObjectCollection,
    type_interactionCollection,
    type_endInteractionCollection,
    type_objectclass,
    type_interactionclass,
    type_int,
    type_dimensions,
    type_time,
    type_tags,
    type_synchronizations,
    type_transportation,
    //type_switches,
    type_dataTypes,
    type_notes,
    type_attribute,
    type_parameter,
    type_unknown = 999,
    type_error = -1
};

struct Entity
{
    const char*     str;
    unsigned int    strLength;
    char                chr;
};

enum
{
    NUM_ENTITY = 5,
    MAX_ENTITY_LENGTH = 6
};

#if 0
static Entity entity[ NUM_ENTITY ];
static bool condenseWhiteSpace;
#endif
static XmlCursor gLocation;

#if 0
/////////////////////////////////////////////////////////////////////////////////////
// String helper function
/////////////////////////////////////////////////////////////////////////////////////
static bool StringEqual( const char* p, const char* tag, bool ignoreCase )
{

    if ( !p || !*p )
    {
        return false;
    }

    const char* q = p;

    if ( ignoreCase )
    {
        while ( *q && *tag && tolower(*q) == tolower(*tag))
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )
            return true;
    }
    else
    {
        while ( *q && *tag && *q == *tag )
        {
            ++q;
            ++tag;
        }

        if ( *tag == 0 )                // Have we found the end of the tag, and everything equal?
            return true;
    }
    return false;
}

static bool IsWhiteSpace( char c )
{
    return ( isspace( (unsigned char) c ) || c == '\n' || c == '\r' );
}

static bool IsWhiteSpace( int c )
{
    if ( c < 256 )
        return IsWhiteSpace( (char) c );
    return false;   // Again, only truly correct for English/Latin...but usually works.
}

static const char* SkipWhiteSpace( const char* p )
{
    if ( !p || !*p )
    {
        return 0;
    }

    while ( *p && IsWhiteSpace( *p ) || *p == '\n' || *p =='\r' )
        ++p;

    return p;
}

static int IsAlphaNum(unsigned char anyByte)
{
    if ( anyByte < 127 )
        return isalnum( anyByte );
    else
        return 1;
}

static const char* GetEntity( const char* p, char* value, int* length )
{
    // Presume an entity, and pull it out.
    int i;
    *length = 0;

    if ( *(p+1) && *(p+1) == '#' && *(p+2) )
    {
        unsigned long ucs = 0;
        ptrdiff_t delta = 0;
        unsigned mult = 1;

        if ( *(p+2) == 'x' )
        {
            // Hexadecimal.
            if ( !*(p+3) ) return 0;

            const char* q = p+3;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != 'x' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else if ( *q >= 'a' && *q <= 'f' )
                    ucs += mult * (*q - 'a' + 10);
                else if ( *q >= 'A' && *q <= 'F' )
                    ucs += mult * (*q - 'A' + 10 );
                else
                    return 0;
                mult *= 16;
                --q;
            }
        }
        else
        {
            // Decimal.
            if ( !*(p+2) ) return 0;

            const char* q = p+2;
            q = strchr( q, ';' );

            if ( !q || !*q ) return 0;

            delta = q-p;
            --q;

            while ( *q != '#' )
            {
                if ( *q >= '0' && *q <= '9' )
                    ucs += mult * (*q - '0');
                else
                    return 0;
                mult *= 10;
                --q;
            }
        }
        *value = (char)ucs;
        *length = 1;

        return p + delta + 1;
    }

    // Now try to match it.
    for ( i=0; i<NUM_ENTITY; ++i )
    {
        if ( strncmp( entity[i].str, p, entity[i].strLength ) == 0 )
        {
            *value = entity[i].chr;
            *length = 1;
            return ( p + entity[i].strLength );
        }
    }

    // So it wasn't an entity, its unrecognized, or something like that.
    *value = *p;    // Don't put back the last one, since we return it!
    //*length = 1;  // Leave unrecognized entities - this doesn't really work.
    // Just writes strange XML.
    return p+1;
}

#endif

#if 0
static const char* GetChar(const char* p, char* _value, int* length)
{

    //for none UTF-8 char
    *length = 1;

    if ( *length == 1 )
    {
        if ( *p == '&' )
            return GetEntity( p, _value, length);
        *_value = *p;
        return p+1;
    }
    else if ( *length )
    {
        // lots of compilers don't like this function (unsafe),
        // and the null terminator isn't needed
        for ( int i=0; p[i] && i<*length; ++i )
        {
            _value[i] = p[i];
        }
        return p + (*length);
    }
    else
    {
        // Not valid text.
        return 0;
    }
}
#endif

/*static int IdentifyNode(const char* p)
{
    std::cout << "in IdentifyNode... => " << *p << " LINE : "<<__LINE__ <<std::endl;
    const char* xmlHeader = { "<?xml" };
    const char* commentHeader = { "<!--" };
    const char* dtdHeader = { "<!" };

    int nodeType = 0;
    
    p = SkipWhiteSpace( p );
    std::cout << "p : " << *p << std::endl;
    if( !p || !*p || *p != '<' )
    {
        std::cout << "type error => " << *p << std::endl;
        return type_error;
    }

    if ( StringEqual(p, xmlHeader, true) )
    {
        // found XML Header
        p = SkipWhiteSpace(p);

        if ( !p || !*p || !StringEqual(p, "<?xml", true) )
        {
            // Not a valid XML format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_xmlHeader;
    }
    else if ( StringEqual(p, commentHeader, false) )
    {
        if ( !p || !*p || !StringEqual(p, "<!--", true) )
        {
            // Not a valid comment format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_commentHeader;
    }
    else if ( StringEqual( p, dtdHeader, false) )
    {
        if ( !p || !*p || !StringEqual(p, "<!", true) )
        {
            // Not a valid comment format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_dtdHeader;
    }
    else if ( StringEqual( p, "<objectModel", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<objectModel", true) )
        {
            // Not a valid comment format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_objectModel;
    }
    else if ( StringEqual( p, "<objects", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<objects", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_objectCollection;
    }
    else if ( StringEqual( p, "<objectClass", false) )
    {
      if ( !p || !*p || !StringEqual(p, "<objectClass", true) )
      {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
        return type_error;
      }
      nodeType = type_objectclass;
    }
    else if ( StringEqual( p, "<attribute", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<attribute", true) )
        {
            // Not a valid comment format....
            //std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        nodeType = type_attribute;
    }
    else if ( StringEqual( p, "<interactions", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<interactions", true) )
        {
            // Not a valid comment format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        std::cout << "find => " << *p << ",type_interactionCollection."<< std::endl;
        nodeType = type_interactionCollection;
    }
    else if ( StringEqual( p, "<interactionClass", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<interactionClass", true) )
        {
            // Not a valid comment format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        std::cout << "find => " << *p << ",type_parameter."<< std::endl;
        nodeType = type_interactionclass;
    }
    else if ( StringEqual( p, "<parameter", false) )
    {
        if ( !p || !*p || !StringEqual(p, "<parameter", true) )
        {
            // Not a valid comment format....
            std::cout << "type error => " << *p << std::endl;
            return type_error;
        }
        std::cout << "find => " << *p << ",type_parameter."<< std::endl;
        nodeType = type_parameter;
    }
    //else if ( isalpha(*(p+1)) || *(p+1) == '_')
    //{
    //    std::cout << "find a node => " << *p << std::endl;
    //    nodeType = type_interactionclass;
    //}
    else
    {
        // Unknown
        std::cout << "type_unknown => " << *p << std::endl;
        nodeType = type_unknown;
    }

    return nodeType;
}
*/
#endif
