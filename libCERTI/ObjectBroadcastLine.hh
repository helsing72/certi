// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: ObjectBroadcastLine.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// Classe privee pour les listes de diffusion.  Les *BroadcastLine
// decrivent une ligne des listes de diffusion.

#ifndef _CERTI_OBJECT_BROADCAST_LINE_HH
#define _CERTI_OBJECT_BROADCAST_LINE_HH

#include "broadcast.hh"

namespace certi {

class  ObjectBroadcastLine {
public:

  FederateHandle  Federate;
  BroadcastState  State [MAX_ATTRIBUTES_PER_CLASS + 1];

  ObjectBroadcastLine(FederateHandle  theFederate,
		      BroadcastState  theInitialState = bsNotSub);
};

}

#endif // _CERTI_OBJECT_BROADCAST_LINE_HH
