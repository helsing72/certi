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
// ----------------------------------------------------------------------------

#ifndef _CERTI_OBJECT_CLASS_SET_HH
#define _CERTI_OBJECT_CLASS_SET_HH

// forward declaration
namespace certi {
class Object;
class SecurityServer;
}  // namespace certi

// CERTI headers
#include "certi.hh"
#include "ObjectClass.hh"
#include "TreeNamedAndHandledSet.hh"

// System headers
#include <string>

namespace certi {

/**
 * This class represents a set of object classes.
 */
class CERTI_EXPORT ObjectClassSet : public TreeNamedAndHandledSet<ObjectClass>
{

public:
	ObjectClassSet(SecurityServer *theSecurityServer, bool isRootClassSet=false);
	~ObjectClassSet();

	/**
	 * Add an object class to the set.
	 * @param[in] theClass the object class to be added
	 * @param[in] parentClass the parent class (may be NULL)
	 */
	void addClass(ObjectClass *theClass,ObjectClass *parentClass);

	// RTI Support Services
	AttributeHandle getAttributeHandle(const std::string& the_name,
			ObjectClassHandle the_class) const;

	const std::string& getAttributeName(AttributeHandle the_handle,
			ObjectClassHandle the_class) const;

	ObjectClassHandle getObjectClassHandle(const std::string&) const;

	const std::string& getObjectClassName(ObjectClassHandle the_handle) const;

	void killFederate(FederateHandle theFederate) noexcept;

    /**
     * Register specified federate as a publisher of the specified attribute list
     * for the specified Object Class.
     * @param[in] theFederateHandle the handle of the publisher federate
     * @param[in] theClassHandle the handle of the class to be published
     * @param[in] theAttributeList the list of attribute of theClassHandle
     * @param[in] PubOrUnpub true when publishing, false when unpublishing
     */
     void publish(FederateHandle theFederateHandle,
                  ObjectClassHandle theClassHandle,
                  const std::vector <AttributeHandle> &theAttributeList,
                  bool PubOrUnpub);

    /**
     * Subscribes a federate to a set of attributes with a region.
     * Sends the discovery messages if necessary.
     * @param federate Federate to subscribe
     * @param class_handle Class to be subscribed
     * @param attributes List of attributes to be subscribed
     * @param region Subscription region (NULL for default)
     */
    void subscribe(FederateHandle, ObjectClassHandle,
                   const std::vector <AttributeHandle>& attributes,
                   const RTIRegion * = NULL);

	// Object Instance Management
	void deleteObject(FederateHandle theFederateHandle,
			Object* object,
			FederationTime theTime,
			const std::string& theTag);

	void deleteObject(FederateHandle theFederateHandle,
			Object* object,
			const std::string& theTag);

	void registerObjectInstance(FederateHandle, Object *, ObjectClassHandle);

	void updateAttributeValues(FederateHandle theFederateHandle,
                        Object* object,
			const std::vector <AttributeHandle> &theAttribArray,
			const std::vector <AttributeValue_t> &theValueArray,
			const FederationTime& theTime,
			const std::string& theUserTag);

	void updateAttributeValues(FederateHandle theFederateHandle,
                        Object* object,
			const std::vector <AttributeHandle> &theAttribArray,
			const std::vector <AttributeValue_t> &theValueArray,
			const std::string& theUserTag);


	// Ownership Management
	void negotiatedAttributeOwnershipDivestiture(FederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &,
			const std::string& theTag);


	void attributeOwnershipAcquisitionIfAvailable(FederateHandle,
			Object* object,
			const std::vector <AttributeHandle>&);

	void unconditionalAttributeOwnershipDivestiture(FederateHandle,
			Object* object,
			const std::vector <AttributeHandle>&);

	void attributeOwnershipAcquisition(FederateHandle theFederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &theAttributeList,
			const std::string& theTag);

	AttributeHandleSet *attributeOwnershipReleaseResponse(FederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &);

	void cancelAttributeOwnershipAcquisition(FederateHandle theFederateHandle,
			Object* object,
			const std::vector <AttributeHandle> &theAttributeList);

private:

	/**
	 * This object will help to find the TCPLink associated with a Federate.
	 * This reference is passed to all new ObjectClass.
	 */
	SecurityServer *server ;

	ObjectClass *getInstanceClass(ObjectHandle theObjectHandle) const;
};

} // namespace certi

#endif // _CERTI_OBJECT_CLASS_SET_HH

// $Id: ObjectClassSet.hh,v 3.43 2013/09/24 14:27:57 erk Exp $
