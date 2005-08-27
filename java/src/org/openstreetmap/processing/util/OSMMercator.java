/*
 * Copyright (C) 2005 Tom Carden (tom@somethingmodern.com)
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 *  
 */

package org.openstreetmap.processing.util;

/** based up OpenStreetMap's Ruby implementation of the Mercator projection, originally by Steve Coast. */

public class OSMMercator {

  protected double clat, clon;
  protected double dlat, dlon;
  protected double degrees_per_pixel;
  protected double tx, ty, bx, by;
  protected double w, h;
  protected double QUARTER_PI = Math.PI / 4.0;
  protected double PIby360 = Math.PI / 360.0;

  protected double xdiv, ydiv;

  // topleft, bottomright 
  protected OSMPoint tl,br;

  /* init me with your centre lat/lon, the number of degrees per pixel and the size of your image */
  public OSMMercator(double clat, double clon, double degrees_per_pixel, int w, int h) {
    this.w = w;
    this.h = h;
    this.degrees_per_pixel = degrees_per_pixel;
    setCentre(clat,clon);
  }

  /* the idea with this is that you can maintain an accurate projection when you pan around an interactive map 
     though obviously any cached points will need re-caching.
     TODO: work out what change in lat/lon is significant enough to warrant re-caching */
  public void setCentre(double clat, double clon) {
  
    this.clat = clat;
    this.clon = clon;
    
    dlon = (w / 2.0) * degrees_per_pixel;
    dlat = (h / 2.0) * degrees_per_pixel * Math.cos(clat * Math.PI / 180.0);

    tx = xsheet(clon - dlon);
    ty = ysheet(clat - dlat);

    bx = xsheet(clon + dlon);
    by = ysheet(clat + dlat);

    tl = new OSMPoint(clat + dlat,clon - dlon);
    br = new OSMPoint(clat - dlat,clon + dlon);

    xdiv = 1.0 / (bx - tx) * w;
    ydiv = 1.0 / (by - ty) * h;
    
  }

  public double kilometersPerPixel() {
    return (40008.0 / 360.0) * degrees_per_pixel;
  }

  // the following two functions will give you the x/y on the entire sheet
  // FIXME: Steve you should explain this a bit

  public double ysheet(double lat) {
    return Math.log(Math.tan(QUARTER_PI + (lat * PIby360)));
  }
  
  public double xsheet(double lon) {
    return lon;
  }

  // and these two will give you the right points on your image. all the constants can be reduced to speed things up. FIXME

  public double y(double lat) {
    return h - ((ysheet(lat) - ty) * ydiv);
  }

  public double x(double lon) {
    return (xsheet(lon) - tx) * xdiv;
  }

  // this is my attempt at a reverse transform... we'll see, TomC
  
  public double lat(double y) {
    return iysheet(((h - y) / ydiv) + ty);
  }
  
  public double lon(double x) {
    return (x / xdiv) + tx;
  }

  // does the inverse of ysheet, whatever that does
  public double iysheet(double y) {
    return (Math.atan(Math.pow(Math.E,y)) - QUARTER_PI)/PIby360;
  }


  // is a point inside width and height?

  public boolean projectable(double lat, double lon) {
    // top left?                                     bottom right?
    return lat < clat + dlat && lon > clon - dlon && lat > clat - dlat && lon < clon + dlon;
  }

  public OSMPoint getTopLeft() {
    return tl;
  }
  
  public OSMPoint getBottomRight() {
    return br;
  }
  
  // TODO transform convenience methods for Point2f->OSMPoint and back again

}


