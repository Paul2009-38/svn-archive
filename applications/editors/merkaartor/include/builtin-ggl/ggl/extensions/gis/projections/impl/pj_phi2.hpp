#ifndef _PROJECTIONS_PHI2_HPP
#define _PROJECTIONS_PHI2_HPP

// Generic Geometry Library - projections (based on PROJ4)
// This file is manually converted from PROJ4

// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Geometry Library by Barend Gehrels (Geodan, Amsterdam)

// Original copyright notice:

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


namespace ggl { namespace projection { namespace detail {

namespace phi2
{
    static const double TOL = 1.0e-10;
    static const int N_ITER = 15;
}

inline double pj_phi2(double ts, double e)
{
    double eccnth, Phi, con, dphi;
    int i;

    eccnth = .5 * e;
    Phi = HALFPI - 2. * atan (ts);
    i = phi2::N_ITER;
    do {
        con = e * sin (Phi);
        dphi = HALFPI - 2. * atan (ts * pow((1. - con) /
           (1. + con), eccnth)) - Phi;
        Phi += dphi;
    } while ( fabs(dphi) > phi2::TOL && --i);
    if (i <= 0)
        throw proj_exception(-18);
    return Phi;
}

}}} // namespace ggl::projection::impl
#endif
