// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
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
// $Id: FederationManagement.cc,v 3.9 2003/05/09 00:27:17 breholee Exp $
// ----------------------------------------------------------------------------

#include "FederationManagement.hh"

namespace certi {
namespace rtia {

static pdCDebug D("RTIA_FM", "(RTIA FM) ");

// ----------------------------------------------------------------------------
//! FederationManagement.
FederationManagement::FederationManagement(Communications *GC)
    : savingState(false), restoringState(false)
{
    comm = GC ;
    tm = NULL ;

    _numero_federation = 0 ;
    federate = 0 ;

    _fin_execution = RTI_FALSE ;

    _est_createur_federation = RTI_FALSE ;
    _est_membre_federation = RTI_FALSE ;

    _nom_federation[0] = 0 ;
    _nom_federe[0] = 0 ;
}


// -------------------------
// -- ~FederationManagement --
// -------------------------

FederationManagement::~FederationManagement()
{
    TypeException e ;

    if (_est_membre_federation) {
        resignFederationExecution(DELETE_OBJECTS, e);
    }

    // BUG: On devrait pouvoir quitter une federation que l'on a cree
    // sans la detruire en partant.
    // a-t-on avertit les autres federes de notre depart et donc
    // de la destruction de notre objet par RemoveObject(HARZI)
    // car le Remove Object ne diffuse pas le message
    if (_est_createur_federation) {
        printf("RTIA: Je reste actif pour detruire la federation...\n");

        destroyFederationExecution(_nom_federation, e);
        while (e != e_NO_EXCEPTION)
            {
                sleep(1);
                destroyFederationExecution(_nom_federation, e);
            }
        printf("RTIA: Federation detruite.\n");
    }
}

// ----------------------------------------------------------------------------
//! createFederationExecution.
void
FederationManagement::
createFederationExecution(const char *theName,
                          TypeException &e)
{
    NetworkMessage requete, reponse ;

    D.Out(pdInit, "Creating Federation %s.", theName);

    e = e_NO_EXCEPTION ;

    // BUG: pour l'instant, le federe ne peut creer q'une federation
    if (_est_createur_federation || _est_membre_federation)
        e = e_RTIinternalError ;

    if (e == e_NO_EXCEPTION) {
        requete.type = m_CREATE_FEDERATION_EXECUTION ;
        strcpy(requete.federationName, theName);

        comm->sendMessage(&requete);

        comm->waitMessage(&reponse, m_CREATE_FEDERATION_EXECUTION,
                          federate);

        if (reponse.exception == e_NO_EXCEPTION) {
            strcpy(_nom_federation, theName);
            _numero_federation = reponse.federation ;
            _est_createur_federation = RTI_TRUE ;
            D.Out(pdInit, "est createur");
        }
        else {
            e = reponse.exception ;
            D.Out(pdInit, "deja cree");
        }
    }
}


// ----------------------------------------------------------------------------
//! destroyFederationExecution.
void
FederationManagement::
destroyFederationExecution(const char *theName,
                           TypeException &e)
{
    NetworkMessage requete, reponse ;

    D.Out(pdInit, "Destroy Federation %s.", theName);

    e = e_NO_EXCEPTION ;

    // BUG: On devrait pouvoir detruire une federation meme si on est
    // pas le createur.
    if (!_est_createur_federation || strcmp(theName, _nom_federation))
        e = e_FederationExecutionDoesNotExist ;

    if (e == e_NO_EXCEPTION) {
        requete.type = m_DESTROY_FEDERATION_EXECUTION ;
        requete.federation = _numero_federation ;
        requete.federate = federate ;
        strcpy(requete.federationName, theName);

        printf("EMISSION DE LA REQUETE\n");
        comm->sendMessage(&requete);
        printf("ATTENTE DE LA REPONSE\n");
        comm->waitMessage(&reponse,
                          m_DESTROY_FEDERATION_EXECUTION,
                          federate);

        if (reponse.exception == e_NO_EXCEPTION) {
            _nom_federation[0] = 0 ;
            _numero_federation = 0 ;
            _est_createur_federation = RTI_FALSE ;
            _fin_execution = RTI_TRUE ;
        }
        else
            e = reponse.exception ;
    }
}

// ----------------------------------------------------------------------------
//! joinFederationExecution.
FederateHandle
FederationManagement::
joinFederationExecution(const char *Federate,
                        const char *Federation,
                        TypeException &e)
{
    NetworkMessage requete, reponse ;
    int i, nb ;

    D.Out(pdInit, "Join Federation %s as %s.", Federation, Federate);

    e = e_NO_EXCEPTION ;

    if (_est_createur_federation && strcmp(Federation, _nom_federation))
        e = e_RTIinternalError ;

    if (_est_membre_federation)
        e = e_FederateAlreadyExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        requete.type = m_JOIN_FEDERATION_EXECUTION ;
        strcpy(requete.federationName, Federation);
        strcpy(requete.federateName, Federate);

        requete.bestEffortAddress = comm->getAddress();
        requete.bestEffortPeer = comm->getPort();

        comm->sendMessage(&requete);

        // On attend la reponse du RTIG(de n'importe quel federe)
        comm->waitMessage(&reponse, m_JOIN_FEDERATION_EXECUTION, 0);

        // Si c'est positif, cette reponse contient le nombre de regulateurs.
        // On attend alors un message NULL de chacun d'eux.
        if (reponse.exception == e_NO_EXCEPTION) {
            strcpy(_nom_federation, Federation);
            strcpy(_nom_federe, Federate);

            _numero_federation = reponse.federation ;
            federate = reponse.federate ;
            tm->MyFederateNumber = reponse.federate ;

#ifdef FEDERATION_USES_MULTICAST
            // creation du socket pour la communication best-effort
            comm->CreerSocketMC(reponse.AdresseMulticast, MC_PORT);
#endif

            nb = reponse.numberOfRegulators ;
            for (i=0 ; i<nb ; i++) {
                comm->waitMessage(&reponse, m_MESSAGE_NULL, 0);
                assert(tm != NULL);
                tm->insert(reponse.federate, reponse.date);
            }

            _est_membre_federation = RTI_TRUE ;
            return(federate);
        }
        else
            e = reponse.exception ;
    }

    return(0);
}

// -------------------------------
// -- resignFederationExecution --
// -------------------------------

void
FederationManagement::resignFederationExecution(ResignAction,
                                                TypeException &e)
{
    NetworkMessage msg ;
    TypeException exception = e_NO_EXCEPTION ;

    e = e_NO_EXCEPTION ;

    D.Out(pdInit, "Resign Federation.");

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        tm->StopperAvanceTemps();

        if (tm->requestRegulateurState() == RTI_TRUE)
            tm->setTimeRegulating(RTI_FALSE, exception);

        msg.type = m_RESIGN_FEDERATION_EXECUTION ;
        msg.federation = _numero_federation ;
        msg.federate = federate ;

        comm->sendMessage(&msg);

        _est_membre_federation = RTI_FALSE ;
        _numero_federation = 0 ;
        federate = 0 ;

        // BUG: Voir DestroyFederation ou ~GF.
        if (!_est_createur_federation)
            _fin_execution = RTI_TRUE ;
    }
}

// ----------------------------------------------------------------------------
//! Register synchronization.
void
FederationManagement::registerSynchronization(const char *label,
                                              const char *tag,
                                              TypeException &e)
{
    D.Out(pdProtocol, "RegisterSynchronization.");

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    list<char *>::const_iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            e = e_FederationAlreadyPaused ; // Label already pending.
            exists = true ;
            break ;
        }
    }

    if (!exists)
        synchronizationLabels.push_back(strdup(label));

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NetworkMessage req ;
        req.type = m_REGISTER_FEDERATION_SYNCHRONIZATION_POINT ;
        req.federation = _numero_federation ;
        req.federate = federate ;
        req.setLabel(label);
        req.setTag(tag);
        comm->sendMessage(&req);
    }
}

// ----------------------------------------------------------------------------
//! Unregister synchronization.
void
FederationManagement::unregisterSynchronization(const char *label,
                                                TypeException &e)
{
    D.Out(pdProtocol, "unregisterSynchronization.");

    assert(label != NULL);

    e = e_NO_EXCEPTION ;

    // Find if this label has been requested by federate or RTIG.
    list<char *>::const_iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            // Label already pending.
            exists = true ;
            break ;
        }
    }
    if (!exists)
        e = e_UnknownLabel ;

    if (!_est_membre_federation)
        e = e_FederateNotExecutionMember ;

    if (e == e_NO_EXCEPTION) {
        NetworkMessage req ;

        req.type = m_SYNCHRONIZATION_POINT_ACHIEVED ;
        req.federation = _numero_federation ;
        req.federate = federate ;
        req.setLabel(label);

        comm->sendMessage(&req);
    }
}

// ----------------------------------------------------------------------------
void
FederationManagement::announceSynchronizationPoint(const char *label,
                                                   const char *tag)
{
    D.Out(pdInit, "Announce Synchronization Point \"%s\"(%s).", label, tag);

    Message req, rep ;

    assert(label != NULL);

    req.type = ANNOUNCE_SYNCHRONIZATION_POINT ;
    req.setLabel(label);
    req.setTag(tag);

    // adding label to list of synchronizations to be done.
    list<char *>::const_iterator i = synchronizationLabels.begin();
    bool exists = false ;
    for (; i != synchronizationLabels.end(); i++) {
        if (!strcmp((*i), label)) {
            // label exists (only if initiator).
            exists = true ;
            break ;
        }
    }
    if (!exists)
        synchronizationLabels.push_back(strdup(label));

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void FederationManagement::
synchronizationPointRegistrationSucceeded(const char *label)
{
    D.Out(pdInit, "Synchronization Point Registration Succeeded \"%s\".",
          label);

    Message req, rep ;

    assert(label != NULL);

    req.type = SYNCHRONIZATION_POINT_REGISTRATION_SUCCEEDED ;
    req.setLabel(label);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationSynchronized(const char *label)
{
    D.Out(pdInit, "Federation Synchronized \"%s\".", label);

    Message req, rep ;

    assert(label != NULL);

    req.type = FEDERATION_SYNCHRONIZED ;
    req.setLabel(label);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationSave(const char *label,
                                            FederationTime the_time,
                                            TypeException &e)
{
    D.Out(pdInit, "Request for federation save \"%s\".", label);

    assert(label != 0);

    NetworkMessage req ;
    req.type = m_REQUEST_FEDERATION_SAVE ;
    req.date = the_time ;
    req.setLabel(label);
    comm->sendMessage(&req);

    // Should make sure that RTIG don't have any save or restore recently set.
    // NetworkMessage rep ;
    // comm->waitMessage(&rep, m_REQUEST_FEDERATION_SAVE, federate);
    // e = rep.exception ;
}

// ----------------------------------------------------------------------------
void
FederationManagement::federateSaveBegun(TypeException &e)
{
    D.Out(pdInit, "Beginning federate save.");

    if (!savingState)
        throw SaveNotInitiated("Federation did not initiate saving.");

    NetworkMessage req ;

    req.type = m_FEDERATE_SAVE_BEGUN ;

    comm->sendMessage(&req);
}

// ----------------------------------------------------------------------------
void
FederationManagement::federateSaveStatus(bool status, TypeException &e)
{
    D.Out(pdInit, "Federate %ssaved.", status ? "" : "not ");

    if (!savingState)
        throw SaveNotInitiated("Federation did not initiate saving.");

    NetworkMessage req ;

    req.type = status ? m_FEDERATE_SAVE_COMPLETE : m_FEDERATE_SAVE_NOT_COMPLETE ;

    comm->sendMessage(&req);
}

// ----------------------------------------------------------------------------
void
FederationManagement::initiateFederateSave(const char *label)
{
    D.Out(pdInit, "Initiate a federate save \"%s\".", label);

    savingState = true ;

    Message req, rep ;

    assert(label != 0);

    req.type = INITIATE_FEDERATE_SAVE ;
    req.setLabel(label);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationSavedStatus(bool status)
{
    D.Out(pdInit, "Federation %ssaved.", status ? "" : "not ");

    savingState = false ;

    Message req, rep ;

    req.type = status ? FEDERATION_SAVED : FEDERATION_NOT_SAVED ;

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationRestore(const char *label,
                                               TypeException &e)
{
    D.Out(pdInit, "Request for federation restore \"%s\".", label);

    assert(label != NULL);

    NetworkMessage req ;
    req.type = m_REQUEST_FEDERATION_RESTORE ;
    req.setLabel(label);
    comm->sendMessage(&req);

    // Should make sure that RTIG don't have any save or restore recently set.
    // ...
}

// ----------------------------------------------------------------------------
void
FederationManagement::federateRestoreStatus(bool status, TypeException &e)
{
    D.Out(pdInit, "Federate %srestored.", status ? "" : "not ");

    if (!restoringState)
        throw RestoreNotRequested("Federation did not initiate restoring.");

    NetworkMessage req ;

    if (status)
        req.type = m_FEDERATE_RESTORE_COMPLETE ;
    else
        req.type = m_FEDERATE_RESTORE_NOT_COMPLETE ;

    comm->sendMessage(&req);
}

// ----------------------------------------------------------------------------
void
FederationManagement::requestFederationRestoreStatus(bool status,
                                                     const char *label,
                                                     const char *reason)
{
    D.Out(pdInit, "Federation restore request %saccepted",
          status ? "" : "not ");

    Message req, rep ;

    req.setLabel(label);

    if (status)
        req.type = REQUEST_FEDERATION_RESTORE_SUCCEEDED ;
    else {
        req.type = REQUEST_FEDERATION_RESTORE_FAILED ;
        req.setTag(reason);
    }

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationRestoreBegun()
{
    D.Out(pdInit, "Federation restore begun");

    Message req, rep ;
    req.type = FEDERATION_RESTORE_BEGUN ;

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::initiateFederateRestore(const char *label,
                                              FederateHandle handle)
{
    D.Out(pdInit, "Initiate federate restore \"%s\" with federate handle %d.",
          label, handle);

    restoringState = true ;

    Message req, rep ;
    req.type = INITIATE_FEDERATE_RESTORE ;
    req.setFederate(handle);
    req.setLabel(label);

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::federationRestoredStatus(bool status)
{
    D.Out(pdInit, "Federation %srestored.", status ? "" : "not ");

    restoringState = false ;

    Message req, rep ;

    if (status)
        req.type = FEDERATION_RESTORED ;
    else
        req.type = FEDERATION_NOT_RESTORED ;

    comm->requestFederateService(&req, &rep);
}

// ----------------------------------------------------------------------------
void
FederationManagement::checkFederationSaving()
    throw (SaveInProgress)
{
    if (savingState) {
        throw SaveInProgress("Federation is in saving state");
    }
}

// ----------------------------------------------------------------------------
void
FederationManagement::checkFederationRestoring()
    throw (RestoreInProgress)
{
    if (restoringState) {
        throw RestoreInProgress("Federation is in restoring state");
    }
}

}} // namespace certi/rtia

// $Id: FederationManagement.cc,v 3.9 2003/05/09 00:27:17 breholee Exp $
