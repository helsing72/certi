// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2003  ONERA
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
// $Id: Extent.cc,v 3.5 2005/03/11 13:32:46 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "Extent.hh"

using std::vector ;

namespace certi {

// ----------------------------------------------------------------------------
/** Extent constructor
    \param n Number of dimensions in the routing space
 */
Extent::Extent(size_t n)
{
    ranges.resize(n);
}

// ----------------------------------------------------------------------------
/** Get range lower bound
 */
ULong
Extent::getRangeLowerBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds();
    else
	return ranges[handle - 1].first ;	
}

// ----------------------------------------------------------------------------
/** Get range upper bound
 */
ULong
Extent::getRangeUpperBound(DimensionHandle handle) const
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds();
    else
	return ranges[handle - 1].second ;
}

// ----------------------------------------------------------------------------
/** Set range lower bound
 */
void
Extent::setRangeLowerBound(DimensionHandle handle, ULong val)
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds();
    else
	ranges[handle - 1].first = val ;
}

// ----------------------------------------------------------------------------
/** Set range upper bound
 */
void
Extent::setRangeUpperBound(DimensionHandle handle, ULong val)
    throw (ArrayIndexOutOfBounds)
{
    if ((handle <= 0) || (handle > ranges.size())) 
	throw ArrayIndexOutOfBounds();
    else
	ranges[handle - 1].second = val ;
}

// ----------------------------------------------------------------------------
/** Get the number of ranges in this Extent.
 */
size_t
Extent::size() const
{
    return ranges.size();
}

// ----------------------------------------------------------------------------
/** Check whether both extents overlap
 */
bool
Extent::overlaps(const Extent &e) const
{
    for (unsigned int i = 1 ; i <= size(); ++i) {
	if (e.getRangeLowerBound(i) > getRangeUpperBound(i) ||
	    e.getRangeUpperBound(i) < getRangeLowerBound(i))
	    return false ;
    }
    return true ;
}

} // namespace certi

// $Id: Extent.cc,v 3.5 2005/03/11 13:32:46 breholee Exp $
