// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2004  ONERA
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

#ifndef LIBCERTI_NAME_RESERVATION_HH
#define LIBCERTI_NAME_RESERVATION_HH

#include "Handled.hh"
#include "Named.hh"
#include <include/certi.hh>
#include <map>

namespace certi {

class NameReservation;

typedef std::map<std::string, NameReservation*> NameReservationSet;

class CERTI_EXPORT NameReservation : public Named, public Handled<FederateHandle> {
public:
    NameReservation(FederateHandle the_owner, std::string the_name)
        : Named(the_name), Handled<FederateHandle>(the_owner)
    {
    }

    virtual ~NameReservation()
    {
    }
};

} // namespace certi
#endif // LIBCERTI_NAME_RESERVATION_HH
