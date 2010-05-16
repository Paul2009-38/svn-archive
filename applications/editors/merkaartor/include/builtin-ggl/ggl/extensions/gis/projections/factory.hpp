// Generic Geometry Library
//
// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GGL_PROJECTIONS_FACTORY_HPP
#define GGL_PROJECTIONS_FACTORY_HPP

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

#include <ggl/extensions/gis/projections/impl/factory_entry.hpp>
#include <ggl/extensions/gis/projections/parameters.hpp>
#include <ggl/extensions/gis/projections/proj/aea.hpp>
#include <ggl/extensions/gis/projections/proj/aeqd.hpp>
#include <ggl/extensions/gis/projections/proj/airy.hpp>
#include <ggl/extensions/gis/projections/proj/aitoff.hpp>
#include <ggl/extensions/gis/projections/proj/august.hpp>
#include <ggl/extensions/gis/projections/proj/bacon.hpp>
#include <ggl/extensions/gis/projections/proj/bipc.hpp>
#include <ggl/extensions/gis/projections/proj/boggs.hpp>
#include <ggl/extensions/gis/projections/proj/bonne.hpp>
#include <ggl/extensions/gis/projections/proj/cass.hpp>
#include <ggl/extensions/gis/projections/proj/cc.hpp>
#include <ggl/extensions/gis/projections/proj/cea.hpp>
#include <ggl/extensions/gis/projections/proj/chamb.hpp>  // control points XY
#include <ggl/extensions/gis/projections/proj/collg.hpp>
#include <ggl/extensions/gis/projections/proj/crast.hpp>
#include <ggl/extensions/gis/projections/proj/denoy.hpp>
#include <ggl/extensions/gis/projections/proj/eck1.hpp>
#include <ggl/extensions/gis/projections/proj/eck2.hpp>
#include <ggl/extensions/gis/projections/proj/eck3.hpp>
#include <ggl/extensions/gis/projections/proj/eck4.hpp>
#include <ggl/extensions/gis/projections/proj/eck5.hpp>
#include <ggl/extensions/gis/projections/proj/eqc.hpp>
#include <ggl/extensions/gis/projections/proj/eqdc.hpp>
#include <ggl/extensions/gis/projections/proj/fahey.hpp>
#include <ggl/extensions/gis/projections/proj/fouc_s.hpp>
#include <ggl/extensions/gis/projections/proj/gall.hpp>
#include <ggl/extensions/gis/projections/proj/geocent.hpp>
#include <ggl/extensions/gis/projections/proj/geos.hpp>
#include <ggl/extensions/gis/projections/proj/gins8.hpp>
#include <ggl/extensions/gis/projections/proj/gn_sinu.hpp>
#include <ggl/extensions/gis/projections/proj/gnom.hpp>
#include <ggl/extensions/gis/projections/proj/goode.hpp> // includes two other projections
#include <ggl/extensions/gis/projections/proj/gstmerc.hpp>
#include <ggl/extensions/gis/projections/proj/hammer.hpp>
#include <ggl/extensions/gis/projections/proj/hatano.hpp>
#include <ggl/extensions/gis/projections/proj/krovak.hpp>
#include <ggl/extensions/gis/projections/proj/imw_p.hpp> // xy functions after inverse
#include <ggl/extensions/gis/projections/proj/laea.hpp>
#include <ggl/extensions/gis/projections/proj/labrd.hpp>
#include <ggl/extensions/gis/projections/proj/lagrng.hpp>
#include <ggl/extensions/gis/projections/proj/larr.hpp>
#include <ggl/extensions/gis/projections/proj/lask.hpp>
#include <ggl/extensions/gis/projections/proj/latlong.hpp>
#include <ggl/extensions/gis/projections/proj/lcc.hpp>
#include <ggl/extensions/gis/projections/proj/lcca.hpp>
#include <ggl/extensions/gis/projections/proj/loxim.hpp>
#include <ggl/extensions/gis/projections/proj/lsat.hpp>
#include <ggl/extensions/gis/projections/proj/mbtfpp.hpp>
#include <ggl/extensions/gis/projections/proj/mbtfpq.hpp>
#include <ggl/extensions/gis/projections/proj/mbt_fps.hpp>
#include <ggl/extensions/gis/projections/proj/merc.hpp>
#include <ggl/extensions/gis/projections/proj/mill.hpp>
#include <ggl/extensions/gis/projections/proj/mod_ster.hpp>
#include <ggl/extensions/gis/projections/proj/moll.hpp>
#include <ggl/extensions/gis/projections/proj/nell.hpp>
#include <ggl/extensions/gis/projections/proj/nell_h.hpp>
#include <ggl/extensions/gis/projections/proj/nocol.hpp>
#include <ggl/extensions/gis/projections/proj/nsper.hpp>
#include <ggl/extensions/gis/projections/proj/nzmg.hpp>
#include <ggl/extensions/gis/projections/proj/ob_tran.hpp> // includes other projection
#include <ggl/extensions/gis/projections/proj/ocea.hpp>
#include <ggl/extensions/gis/projections/proj/oea.hpp>
#include <ggl/extensions/gis/projections/proj/omerc.hpp>
#include <ggl/extensions/gis/projections/proj/ortho.hpp>
#include <ggl/extensions/gis/projections/proj/poly.hpp>
#include <ggl/extensions/gis/projections/proj/putp2.hpp>
#include <ggl/extensions/gis/projections/proj/putp3.hpp>
#include <ggl/extensions/gis/projections/proj/putp4p.hpp>
#include <ggl/extensions/gis/projections/proj/putp5.hpp>
#include <ggl/extensions/gis/projections/proj/putp6.hpp>
#include <ggl/extensions/gis/projections/proj/robin.hpp>
#include <ggl/extensions/gis/projections/proj/rouss.hpp>
#include <ggl/extensions/gis/projections/proj/rpoly.hpp>
#include <ggl/extensions/gis/projections/proj/sconics.hpp>
#include <ggl/extensions/gis/projections/proj/somerc.hpp>
#include <ggl/extensions/gis/projections/proj/stere.hpp>
#include <ggl/extensions/gis/projections/proj/sterea.hpp>
#include <ggl/extensions/gis/projections/proj/sts.hpp>
#include <ggl/extensions/gis/projections/proj/tcc.hpp>
#include <ggl/extensions/gis/projections/proj/tcea.hpp>
#include <ggl/extensions/gis/projections/proj/tmerc.hpp>
#include <ggl/extensions/gis/projections/proj/tpeqd.hpp>
#include <ggl/extensions/gis/projections/proj/urm5.hpp>
#include <ggl/extensions/gis/projections/proj/urmfps.hpp>
#include <ggl/extensions/gis/projections/proj/vandg.hpp>
#include <ggl/extensions/gis/projections/proj/vandg2.hpp>
#include <ggl/extensions/gis/projections/proj/vandg4.hpp>
#include <ggl/extensions/gis/projections/proj/wag2.hpp>
#include <ggl/extensions/gis/projections/proj/wag3.hpp>
#include <ggl/extensions/gis/projections/proj/wag7.hpp>
#include <ggl/extensions/gis/projections/proj/wink1.hpp>
#include <ggl/extensions/gis/projections/proj/wink2.hpp>

namespace ggl { namespace projection
{

template <typename LatLong, typename Cartesian, typename Parameters = parameters>
class factory : public detail::base_factory<LatLong, Cartesian, Parameters>
{
private:

    typedef std::map<std::string, boost::shared_ptr<detail::factory_entry<LatLong, Cartesian, Parameters> > > prj_registry;
    prj_registry m_registry;

public:

    factory()
    {
        detail::aea_init(*this);
        detail::aeqd_init(*this);
        detail::airy_init(*this);
        detail::aitoff_init(*this);
        detail::august_init(*this);
        detail::bacon_init(*this);
        detail::bipc_init(*this);
        detail::boggs_init(*this);
        detail::bonne_init(*this);
        detail::cass_init(*this);
        detail::cc_init(*this);
        detail::cea_init(*this);
        detail::chamb_init(*this);
        detail::collg_init(*this);
        detail::crast_init(*this);
        detail::denoy_init(*this);
        detail::eck1_init(*this);
        detail::eck2_init(*this);
        detail::eck3_init(*this);
        detail::eck4_init(*this);
        detail::eck5_init(*this);
        detail::eqc_init(*this);
        detail::eqdc_init(*this);
        detail::fahey_init(*this);
        detail::fouc_s_init(*this);
        detail::gall_init(*this);
        detail::geocent_init(*this);
        detail::geos_init(*this);
        detail::gins8_init(*this);
        detail::gn_sinu_init(*this);
        detail::gnom_init(*this);
        detail::goode_init(*this);
        detail::gstmerc_init(*this);
        detail::hammer_init(*this);
        detail::hatano_init(*this);
        detail::krovak_init(*this);
        detail::imw_p_init(*this);
        detail::labrd_init(*this);
        detail::laea_init(*this);
        detail::lagrng_init(*this);
        detail::larr_init(*this);
        detail::lask_init(*this);
        detail::latlong_init(*this);
        detail::lcc_init(*this);
        detail::lcca_init(*this);
        detail::loxim_init(*this);
        detail::lsat_init(*this);
        detail::mbtfpp_init(*this);
        detail::mbtfpq_init(*this);
        detail::mbt_fps_init(*this);
        detail::merc_init(*this);
        detail::mill_init(*this);
        detail::mod_ster_init(*this);
        detail::moll_init(*this);
        detail::nell_init(*this);
        detail::nell_h_init(*this);
        detail::nocol_init(*this);
        detail::nsper_init(*this);
        detail::nzmg_init(*this);
        detail::ob_tran_init(*this);
        detail::ocea_init(*this);
        detail::oea_init(*this);
        detail::omerc_init(*this);
        detail::ortho_init(*this);
        detail::poly_init(*this);
        detail::putp2_init(*this);
        detail::putp3_init(*this);
        detail::putp4p_init(*this);
        detail::putp5_init(*this);
        detail::putp6_init(*this);
        detail::robin_init(*this);
        detail::rouss_init(*this);
        detail::rpoly_init(*this);
        detail::sconics_init(*this);
        detail::somerc_init(*this);
        detail::stere_init(*this);
        detail::sterea_init(*this);
        detail::sts_init(*this);
        detail::tcc_init(*this);
        detail::tcea_init(*this);
        detail::tmerc_init(*this);
        detail::tpeqd_init(*this);
        detail::urm5_init(*this);
        detail::urmfps_init(*this);
        detail::vandg_init(*this);
        detail::vandg2_init(*this);
        detail::vandg4_init(*this);
        detail::wag2_init(*this);
        detail::wag3_init(*this);
        detail::wag7_init(*this);
        detail::wink1_init(*this);
        detail::wink2_init(*this);
    }

    virtual ~factory() {}

    virtual void add_to_factory(const std::string& name, detail::factory_entry<LatLong, Cartesian, Parameters>* sub)
    {
        m_registry[name].reset(sub);
    }

    inline projection<LatLong, Cartesian>* create_new(const Parameters& parameters)
    {
        typename prj_registry::iterator it = m_registry.find(parameters.name);
        if (it != m_registry.end())
        {
            return it->second->create_new(parameters);
        }

        return 0;
    }
};

}} // namespace ggl::projection

#endif // GGL_PROJECTIONS_FACTORY_HPP
