// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003, 2004  ONERA
//
// This file is part of CERTI
//
// CERTI is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// CERTI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIG_HH
#define CERTI_RTIG_HH

// #include <netinet/in.h>
#include <string>

#include "AuditFile.hh"
#include "BasicMessage.hh"
#include "Exception.hh"
#include "FederationsList.hh"
#include "HandleManager.hh"
#include "MessageBuffer.hh"
#include "NM_Classes.hh" // IWYU pragma: export
// #include "NetworkMessage.hh"
// #include "SecureTCPSocket.hh"
#include "SocketServer.hh"
#include "SocketTCP.hh"
#include "SocketUDP.hh"
#include "certi.hh"

namespace certi {

class NetworkMessage;
class Socket;

namespace rtig {

/**
     * This class manages the RTIG server, aka RTI Gateway server.
     * Each time a message is received, it is
     * processed by a RTI_processing.cc module.
     *
     * These modules called "processXXX" are divided into two parts :
     * <ul>
     *   <li> one part for treating the received message. </li>
     *   <li> another part generating and sending back a response. </li>
     * </li>
     */
class RTIG {
public:
    RTIG();
    ~RTIG();

    void execute() throw(NetworkError);

    static void signalHandler(int sig);

    void setVerboseLevel(const int level);
    void setListeningIPAddress(const std::string& hostName) throw(NetworkError);

private:
    static bool terminate;

    void createSocketServers();

    /** Choose the right processing module to call.
         *  
         * This module chooses the right processing module to call.
         * This process is done by examinating the message type.
         * 
         * @return the socket, because it may have been closed & deleted in the meantime
         */
    Socket* chooseProcessingMethod(Socket*, NetworkMessage*);

    /** Process incoming messages.
         *
         * This module works as follows:
         * Each processXXX module processes its own answer and any broadcast needed.
         * processXXX module calling is decided by the ChooseProcessingMethod module.
         * But if an exception occurs while processing a message, the exception is
         * caught by this module. Then a message, similar to the received one is sent
         * on the link. This message only holds the exception.
         * 
         * @return the socket, because it may have been closed & deleted in the meantime
         */
    Socket* processIncomingMessage(Socket*) throw(NetworkError);

    void openConnection();

    /** closeConnection
         * 
         * If a connection is closed in emergency, KillFederate will be called on
         * federations attribute to remove all references to this federate.
         */
    void closeConnection(Socket*, bool emergency);

    // Event handlers
    void processCreateFederation(Socket*, NM_Create_Federation_Execution*);
    void processJoinFederation(Socket*, NM_Join_Federation_Execution*);
    void processResignFederation(Socket*, Handle, FederateHandle);
    void processDestroyFederation(Socket*, NM_Destroy_Federation_Execution*);
    void processSetClassRelevanceAdvisorySwitch(Socket*, NM_Set_Class_Relevance_Advisory_Switch*);
    void processSetInteractionRelevanceAdvisorySwitch(Socket*, NM_Set_Interaction_Relevance_Advisory_Switch*);
    void processSetAttributeRelevanceAdvisorySwitch(Socket*, NM_Set_Attribute_Relevance_Advisory_Switch*);
    void processSetAttributeScopeAdvisorySwitch(Socket*, NM_Set_Attribute_Scope_Advisory_Switch*);
    void processSetTimeRegulating(Socket*, NM_Set_Time_Regulating* msg);
    void processSetTimeConstrained(Socket*, NM_Set_Time_Constrained* msg);
    void processMessageNull(NetworkMessage* msg, bool anonymous);
    void processMessageNullPrime(NM_Message_Null_Prime* msg);
    void processRegisterSynchronization(Socket*, NM_Register_Federation_Synchronization_Point*);
    void processSynchronizationAchieved(Socket*, NetworkMessage*);
    void processRequestFederationSave(Socket*, NetworkMessage*);
    void processFederateSaveBegun(Socket*, NetworkMessage*);
    void processFederateSaveStatus(Socket*, NetworkMessage*);
    void processRequestFederationRestore(Socket*, NetworkMessage*);
    void processFederateRestoreStatus(Socket*, NetworkMessage*);
    void processPublishObjectClass(Socket*, NM_Publish_Object_Class*);
    void processSubscribeObjectClass(Socket*, NM_Subscribe_Object_Class*);
    void processPublishInteractionClass(Socket*, NM_Publish_Interaction_Class*);
    void processSubscribeInteractionClass(Socket*, NM_Subscribe_Interaction_Class*);
    void processUnpublishInteractionClass(Socket*, NetworkMessage*);
    void processUnsubscribeInteractionClass(Socket*, NetworkMessage* msg);
    void processReserveObjectInstanceName(Socket* link, NM_Reserve_Object_Instance_Name* req);
    void processRegisterObject(Socket*, NM_Register_Object*);
    void processUpdateAttributeValues(Socket*, NM_Update_Attribute_Values*);
    void processSendInteraction(Socket*, NM_Send_Interaction*);
    void processDeleteObject(Socket*, NM_Delete_Object*);
    void processQueryAttributeOwnership(Socket*, NM_Query_Attribute_Ownership*);
    void processNegotiatedOwnershipDivestiture(Socket*, NM_Negotiated_Attribute_Ownership_Divestiture*);
    void processAcquisitionIfAvailable(Socket*, NM_Attribute_Ownership_Acquisition_If_Available*);
    void processUnconditionalDivestiture(Socket*, NM_Unconditional_Attribute_Ownership_Divestiture*);
    void processOwnershipAcquisition(Socket*, NM_Attribute_Ownership_Acquisition*);
    void processCancelNegotiatedDivestiture(Socket*, NM_Cancel_Negotiated_Attribute_Ownership_Divestiture*);
    void processAttributeOwnedByFederate(Socket*, NM_Is_Attribute_Owned_By_Federate*);
    void processReleaseResponse(Socket*, NM_Attribute_Ownership_Release_Response*);
    void processCancelAcquisition(Socket*, NM_Cancel_Attribute_Ownership_Acquisition*);
    void processCreateRegion(Socket*, NM_DDM_Create_Region*);
    void processModifyRegion(Socket*, NM_DDM_Modify_Region*);
    void processDeleteRegion(Socket*, NM_DDM_Delete_Region*);
    void processAssociateRegion(Socket*, NM_DDM_Associate_Region*);
    void processUnassociateRegion(Socket*, NM_DDM_Unassociate_Region*);
    void processSubscribeAttributesWR(Socket*, NM_DDM_Subscribe_Attributes*);
    void processUnsubscribeAttributesWR(Socket*, NM_DDM_Unsubscribe_Attributes*);
    void processSubscribeInteractionWR(Socket*, NM_DDM_Subscribe_Interaction*);
    void processUnsubscribeInteractionWR(Socket*, NM_DDM_Unsubscribe_Interaction*);
    void processRegisterObjectWithRegion(Socket*, NM_DDM_Register_Object*);
    void processRequestObjectAttributeValueUpdate(Socket*, NM_Request_Object_Attribute_Value_Update*);
    void processRequestClassAttributeValueUpdate(Socket*, NM_Request_Class_Attribute_Value_Update*);

private:
    int inferTcpPort() const;
    int inferUdpPort() const;

    int my_tcpPort;
    int my_udpPort;
    int my_verboseLevel;
    in_addr_t my_listeningIPAddress;
    HandleManager<Handle> my_federationHandles;
    SocketTCP my_tcpSocketServer;
    SocketUDP my_udpSocketServer;
    SocketServer my_socketServer;
    AuditFile my_auditServer;
    FederationsList my_federations;
    /** The message buffer used to send Network messages */
    MessageBuffer my_NM_msgBufSend;
    /** The message buffer used to receive Network messages */
    MessageBuffer my_NM_msgBufReceive;
};
}
} // namespaces

#endif // CERTI_RTIG_HH

// $Id: RTIG.hh,v 3.39 2013/09/24 14:27:58 erk Exp $
