// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2005  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: Object.hh,v 3.20 2008/12/07 20:16:14 gotthardp Exp $
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_HH
#define _CERTI_OBJECT_HH

// forward declaration
namespace certi {
	class ObjectAttribute;
	class RTIRegion;
}

#include "certi.hh"
#include "Named.hh"
#include "Handled.hh"
#include "Exception.hh"

#include <deque>
#include <list>

namespace certi {

class CERTI_EXPORT Object : public Named, public Handled<ObjectHandle>
{
public:
    Object(FederateHandle the_owner)
        : Owner(the_owner), name("") { handle = 0 ; };

    Object(FederateHandle the_owner, const char *the_name);

    virtual ~Object();

    void display() const ;

    void addAttribute(ObjectAttribute * new_attribute);
    ObjectAttribute *getAttribute(AttributeHandle the_attribute) const
        throw (AttributeNotDefined);

    bool isAttributeOwnedByFederate(FederateHandle, AttributeHandle) const
        throw (AttributeNotDefined, RTIinternalError);

    ObjectClassHandle getClass() const ;
    void setClass(ObjectClassHandle h);

    FederateHandle getOwner() const ;
    void setOwner(FederateHandle);

    void unassociate(RTIRegion *);

    // -----------------------
    // -- Public Attributes --
    // ----------------------
    /*! Owner Handle
      BUG: Should be handled at the attribute level, not instance level.
    */
    FederateHandle Owner ;

private:
    //! Attribute list from object class instance (private).
    std::deque<ObjectAttribute *> attributeState ;

    ObjectHandle handle ; //!< Object Instance ID
    ObjectClassHandle classHandle ; //! Object Class
    ObjectName_t name ; //!< Instance name.

    /*! federate list subscribed to this class and with subscription region
      intersect with UR.
    */
    std::list<FederateHandle *> sf ;
};

}

#endif // _CERTI_OBJECT_HH

// $Id: Object.hh,v 3.20 2008/12/07 20:16:14 gotthardp Exp $
