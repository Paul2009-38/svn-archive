/**
 * This program is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the 
 * Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

package org.openstreetmap.josm.plugins.elevation.gui;

import static org.openstreetmap.josm.tools.I18n.tr;

import java.awt.Graphics2D;
import javax.swing.Action;
import javax.swing.Icon;

import org.openstreetmap.josm.Main;
import org.openstreetmap.josm.data.Bounds;
import org.openstreetmap.josm.data.gpx.WayPoint;
import org.openstreetmap.josm.data.osm.visitor.BoundingXYVisitor;
import org.openstreetmap.josm.gui.MapView;
import org.openstreetmap.josm.gui.dialogs.LayerListPopup;
import org.openstreetmap.josm.gui.layer.Layer;
import org.openstreetmap.josm.plugins.elevation.ElevationWayPointKind;
import org.openstreetmap.josm.plugins.elevation.IElevationProfile;
import org.openstreetmap.josm.plugins.elevation.WayPointHelper;
import org.openstreetmap.josm.tools.ImageProvider;

/**
 * Layer class to show additional information on the elevation map, e. g. show
 * min/max elevation markers.
 * 
 * @author Oliver
 * 
 */
public class ElevationProfileLayer extends
		org.openstreetmap.josm.gui.layer.Layer {
	private IElevationProfile profile;
	private IElevationProfileRenderer renderer = new DefaultElevationProfileRenderer();

	/**
	 * Creates a new elevation profile layer
	 * 
	 * @param name
	 *            The name of the layer.
	 */
	public ElevationProfileLayer(String name) {
		super(name);
	}

	/**
	 * Gets the current elevation profile shown in this layer.
	 * 
	 * @return
	 */
	public IElevationProfile getProfile() {
		return profile;
	}

	/**
	 * Sets the current elevation profile shown in this layer.
	 * 
	 * @param profile
	 *            The profile to show in the layer
	 */
	public void setProfile(IElevationProfile profile) {
		this.profile = profile;
		Main.map.repaint();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.openstreetmap.josm.gui.layer.Layer#getIcon()
	 */
	@Override
	public Icon getIcon() {
		return ImageProvider.get("layer", "marker_small");
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.openstreetmap.josm.gui.layer.Layer#getInfoComponent()
	 */
	@Override
	public Object getInfoComponent() {
		return getToolTipText();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.openstreetmap.josm.gui.layer.Layer#getMenuEntries()
	 */
	@Override
	public Action[] getMenuEntries() {
		// TODO: More entries???
		return new Action[] { new LayerListPopup.InfoAction(this) };
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.openstreetmap.josm.gui.layer.Layer#getToolTipText()
	 */
	@Override
	public String getToolTipText() {
		if (profile != null) {
			return tr("Elevation profile for track '" + profile.getName() + "'.");
		} else {
			return tr("Elevation profile");
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.openstreetmap.josm.gui.layer.Layer#isMergable(org.openstreetmap.josm
	 * .gui.layer.Layer)
	 */
	@Override
	public boolean isMergable(Layer other) {
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.openstreetmap.josm.gui.layer.Layer#mergeFrom(org.openstreetmap.josm
	 * .gui.layer.Layer)
	 */
	@Override
	public void mergeFrom(Layer from) {
		// nothing to do
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.openstreetmap.josm.gui.layer.Layer#paint(java.awt.Graphics2D,
	 * org.openstreetmap.josm.gui.MapView, org.openstreetmap.josm.data.Bounds)
	 */
	@Override
	public void paint(Graphics2D g, MapView mv, Bounds box) {
		WayPoint lastWpt = null;
		int lastEle = 0;

		if (profile != null) {
			for (WayPoint wpt : profile.getWayPoints()) {
				int ele = (int) WayPointHelper.getElevation(wpt);

				if (lastWpt != null) {
					int h1 = WayPointHelper.getHourOfWayPoint(wpt);
					int h2 = WayPointHelper.getHourOfWayPoint(lastWpt);
					if (h1 != h2) { // hour changed?
						renderer.renderWayPoint(g, profile, mv, wpt,
								ElevationWayPointKind.FullHour);
					} else { // check for elevation gain
						if (ele > lastEle) {
							renderer.renderWayPoint(g, profile, mv, wpt,
									ElevationWayPointKind.ElevationGain);
						} else {
							renderer.renderWayPoint(g, profile, mv, wpt,
									ElevationWayPointKind.ElevationLoss);
						}
					}
				}
				
				// remember for next iteration
				lastEle = (int) WayPointHelper.getElevation(wpt);
				lastWpt = wpt;
			}

			renderer.renderWayPoint(g, profile, mv, profile.getStartWayPoint(),
					ElevationWayPointKind.StartPoint);
			renderer.renderWayPoint(g, profile, mv, profile.getEndWayPoint(),
					ElevationWayPointKind.EndPoint);
			renderer.renderWayPoint(g, profile, mv, profile.getMaxWayPoint(),
					ElevationWayPointKind.MaxElevation);
			renderer.renderWayPoint(g, profile, mv, profile.getMinWayPoint(),
					ElevationWayPointKind.MinElevation);
		} else {
			System.err.println("Layer#paint: No profile");
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * org.openstreetmap.josm.gui.layer.Layer#visitBoundingBox(org.openstreetmap
	 * .josm.data.osm.visitor.BoundingXYVisitor)
	 */
	@Override
	public void visitBoundingBox(BoundingXYVisitor v) {
		// TODO Auto-generated method stub
	}

}
