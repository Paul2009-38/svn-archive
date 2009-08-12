#ifndef GGL_PROJECTIONS_CEA_HPP
#define GGL_PROJECTIONS_CEA_HPP

// Generic Geometry Library - projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright Barend Gehrels (1995-2009), Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande (2008-2009)
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

#include <boost/math/special_functions/hypot.hpp>

#include <ggl/projections/impl/base_static.hpp>
#include <ggl/projections/impl/base_dynamic.hpp>
#include <ggl/projections/impl/projects.hpp>
#include <ggl/projections/impl/factory_entry.hpp>
#include <ggl/projections/impl/pj_qsfn.hpp>
#include <ggl/projections/impl/pj_auth.hpp>

namespace ggl { namespace projection
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace cea{ 
            static const double EPS = 1e-10;

            struct par_cea
            {
                double qp;
                double apa[APA_SIZE];
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_cea_ellipsoid : public base_t_fi<base_cea_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_cea m_proj_parm;

                inline base_cea_ellipsoid(const Parameters& par)
                    : base_t_fi<base_cea_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                	xy_x = this->m_par.k0 * lp_lon;
                	xy_y = .5 * pj_qsfn(sin(lp_lat), this->m_par.e, this->m_par.one_es) / this->m_par.k0;
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                	lp_lat = pj_authlat(asin( 2. * xy_y * this->m_par.k0 / this->m_proj_parm.qp), this->m_proj_parm.apa);
                	lp_lon = xy_x / this->m_par.k0;
                }
            };

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_cea_spheroid : public base_t_fi<base_cea_spheroid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                par_cea m_proj_parm;

                inline base_cea_spheroid(const Parameters& par)
                    : base_t_fi<base_cea_spheroid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                	xy_x = this->m_par.k0 * lp_lon;
                	xy_y = sin(lp_lat) / this->m_par.k0;
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                	double t;
                
                	if ((t = fabs(xy_y *= this->m_par.k0)) - EPS <= 1.) {
                		if (t >= 1.)
                			lp_lat = xy_y < 0. ? -HALFPI : HALFPI;
                		else
                			lp_lat = asin(xy_y);
                		lp_lon = xy_x / this->m_par.k0;
                	} else throw proj_exception();;
                }
            };

            // Equal Area Cylindrical
            template <typename Parameters>
            void setup_cea(Parameters& par, par_cea& proj_parm)
            {
            	double t;
            	if (pj_param(par.params, "tlat_ts").i &&
            		(par.k0 = cos(t = pj_param(par.params, "rlat_ts").f)) < 0.) throw proj_exception(-24);
            	else
            		t = 0.;
            	if (par.es) {
            		t = sin(t);
            		par.k0 /= sqrt(1. - par.es * t * t);
            		par.e = sqrt(par.es);
                    pj_authset(par.es, proj_parm.apa);
            		proj_parm.qp = pj_qsfn(1., par.e, par.one_es);
                // par.inv = e_inverse;
                // par.fwd = e_forward;
            	} else {
                // par.inv = s_inverse;
                // par.fwd = s_forward;
            	}
            }

        }} // namespace detail::cea
    #endif // doxygen 

    /*!
        \brief Equal Area Cylindrical projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Cylindrical
         - Spheroid
         - Ellipsoid
         - lat_ts=
        \par Example
        \image html ex_cea.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct cea_ellipsoid : public detail::cea::base_cea_ellipsoid<Geographic, Cartesian, Parameters>
    {
        inline cea_ellipsoid(const Parameters& par) : detail::cea::base_cea_ellipsoid<Geographic, Cartesian, Parameters>(par)
        {
            detail::cea::setup_cea(this->m_par, this->m_proj_parm);
        }
    };

    /*!
        \brief Equal Area Cylindrical projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Cylindrical
         - Spheroid
         - Ellipsoid
         - lat_ts=
        \par Example
        \image html ex_cea.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct cea_spheroid : public detail::cea::base_cea_spheroid<Geographic, Cartesian, Parameters>
    {
        inline cea_spheroid(const Parameters& par) : detail::cea::base_cea_spheroid<Geographic, Cartesian, Parameters>(par)
        {
            detail::cea::setup_cea(this->m_par, this->m_proj_parm);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Factory entry(s)
        template <typename Geographic, typename Cartesian, typename Parameters>
        class cea_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    if (par.es)
                        return new base_v_fi<cea_ellipsoid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                    else
                        return new base_v_fi<cea_spheroid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        inline void cea_init(detail::base_factory<Geographic, Cartesian, Parameters>& factory)
        {
            factory.add_to_factory("cea", new cea_entry<Geographic, Cartesian, Parameters>);
        }

    } // namespace detail 
    #endif // doxygen

}} // namespace ggl::projection

#endif // GGL_PROJECTIONS_CEA_HPP

