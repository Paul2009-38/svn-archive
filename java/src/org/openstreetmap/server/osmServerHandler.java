/*
   Copyright (C) 2004 Stephen Coast (steve@fractalus.com)

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 */

package org.openstreetmap.server;

import java.util.*;
import java.lang.*;
import java.net.*;
import java.io.*;
import org.apache.xmlrpc.*;

import org.openstreetmap.util.gpspoint;

public class osmServerHandler
{
  private String sJDBC = "jdbc:mysql://128.40.59.181/openstreetmap?useUnicode=true&characterEncoding=latin1";

  
  private osmServerSQLHandler osmSQLH;

  public osmServerHandler()
  {

  
    osmSQLH = new osmServerSQLHandler(sJDBC, "openstreetmap","openstreetmap");

    
  } // osmServerSQLHandler
  

  
  public boolean SQLConnectSuccess()
  {
    return osmSQLH.SQLConnectSuccess();

  } // SQLConnectSuccess
  
 
  public int largestTrackID(String token)
  {
    return osmSQLH.largestTrackID(token);

  } // largestTrackID


  public String login(String user, String pass)
  {
    return( osmSQLH.login(user,pass) );

     
  } // login
 

 
  public String addUser(String user, String pass)
  {
    
    return  osmSQLH.addUser(user,pass);

     
  } // addUser


 
 
  public boolean confirmUser(String user, String token)
  {
    
    return  osmSQLH.confirmUser(user,token);

     
  } // addUser

  
  public boolean userExists(String user)
  {
    
    return  osmSQLH.userExists(user);

     
  } // addUser

 
 
  public Integer addNewStreet(String token,
      String sStreetName,
      double lat1,
      double lon1,
      double lat2,
      double lon2
      )
  {

 
    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return new Integer(-1);
    }

    return osmSQLH.addNewStreet(
        sStreetName,
        (float)lat1,
        (float)lon1,
        (float)lat2,
        (float)lon2,
        uid);

  } // addNewStreet



  public boolean addStreetSegment(String token,
      int street_uid,
      double lat1,
      double lon1,
      double lat2,
      double lon2
      )
  {

 
    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;
    }

    return osmSQLH.addStreetSegment(
        street_uid,
        (float)lat1,
        (float)lon1,
        (float)lat2,
        (float)lon2,
        uid);

  } // addStreetSegment




  public boolean addPoint(String token,
      double lat,
      double lon,
      double alt,
      Date date,
      double hor_dilution,
      double vert_dilution,
      int track_id,
      int quality,
      int satellites
      )
  {
    

//    System.out.println("adding point");
    if(
        date == null
      )
    {
      //FIXME: add more data checks
      return false;

    }

    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;
    }

//    System.out.println("handing over to addPoint sql handler");
  
    return osmSQLH.addPoint(
        (float)lat,
        (float)lon,
        (float)alt,
        date.getTime(),
        (float)hor_dilution,
        (float)vert_dilution,
        track_id,
        quality,
        satellites,
        uid);

  } // addPoint

/* 
  public boolean addPoints(String token,
      double lat[],
      double lon[],
      double alt[],
      Date date[],
      double hor_dilution[],
      double vert_dilution[],
      int track_id[],
      int quality[],
      int satellites[]
      )
  {


    osmServerSQLHandler osmSQLH = new osmServerSQLHandler(sJDBC, "openstreetmap","openstreetmap");

    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;
    }


    long[] times = new long[date.length];
    float[] lats = new float[date.length];
//    float[] lons = new 
    
    for(int i = 0; i < date.length; i++)
    {

      times[i] = date[i].getTime();

    }
    
    return osmSQLH.addPoint(
        (float)lat,
        (float)lon,
        (float)alt,
        times,
        (float)hor_dilution,
        (float)vert_dilution,
        track_id,
        quality,
        satellites,
        uid);

  
  } // addPoints
*/

  public Vector getStreets(
      String token,
      double p1lat,
      double p1lon,
      double p2lat,
      double p2lon)
  {
    try{

      if( !token.equals("applet") && osmSQLH.validateToken(token) == -1 )
      {
        return null;
      }

      Vector v = osmSQLH.getStreets((float)p1lat, (float)p1lon, (float)p2lat, (float)p2lon);

      if( osmSQLH.SQLSuccessful() )
      {

        return v;

      }
      else
      {

        System.out.println("error....");

      }


    }
    catch(Exception e)
    {

      System.out.println(e);
      e.printStackTrace();
      System.exit(-1);

    }

    return null;
  } // getStreets


  public boolean validateToken(String sToken)
  {
    if( osmSQLH.validateToken(sToken) == -1)
    { 
      return false;
    }

    return true;

  } // validateToken
   


  public Vector getPoints(
      String token,
      double p1lat,
      double p1lon,
      double p2lat,
      double p2lon)
  {
    try{

      if( !token.equals("applet") && osmSQLH.validateToken(token) == -1 )
      {
        return null;
      }

      Vector v = osmSQLH.getPoints((float)p1lat, (float)p1lon, (float)p2lat, (float)p2lon);

      Vector results = new Vector();

      if( osmSQLH.SQLSuccessful() )
      {

        Enumeration e = v.elements();

        while(e.hasMoreElements())
        {
          gpspoint g = (gpspoint)e.nextElement();

          results.addElement( new Double(g.getLatitude()) );
          results.addElement( new Double(g.getLongitude()) );

        }


        return results;
      }
      else
      {
        System.out.println("error....");
      }
    }
    catch(Exception e)
    {
      System.out.println(e);
      e.printStackTrace();
      System.exit(-1);

    }

    return null;
  } // getPoints


  public Vector getFullPoints(
      String token,
      double p1lat,
      double p1lon,
      double p2lat,
      double p2lon)
  {
    try{

      Vector v = osmSQLH.getFullPoints((float)p1lat, (float)p1lon, (float)p2lat, (float)p2lon);

      return v;

    }
    catch(Exception e)
    {
      System.out.println(e);
      e.printStackTrace();
      System.exit(-1);

    }

    return null;

  } // getFullPoints


  public boolean dropPoint(String token, double lon, double lat)
  {


    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;
    }

    return osmSQLH.dropPoint(
        (float)lon,
        (float)lat,
        uid);

  } // dropPoint


  public boolean dropPointsInArea(String token, double lon1, double lat1, double lon2, double lat2)
  {

    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;
    }

    return osmSQLH.dropPointsInArea(
        (float)lon1,
        (float)lat1,
        (float)lon2,
        (float)lat2,
        uid);

  } // dropPointsInArea


  public Vector getAllKeys(String token, boolean bVisibleOrNot)
  {
    
    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return new Vector();

    }

    return osmSQLH.getAllKeys(bVisibleOrNot);

  } // getAllKeys


  public Vector getKeyHistory(String token, int nKeyNum)
  {
    
    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return new Vector();

    }

    return osmSQLH.getKeyHistory(nKeyNum);

  } // getKeyHistory
  


  public boolean deleteKey(String token, int nKeyNum)
  {

    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;

    }

    return osmSQLH.deleteKey(nKeyNum, uid);

  } // deleteKey


  
  public boolean undeleteKey(String token, int nKeyNum)
  {

    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;

    }

    return osmSQLH.undeleteKey(nKeyNum, uid);

  } // undeleteKey


  public boolean setNewKeyName(String token, String sNewKeyName, int nKeyNum)
  {
    int uid = osmSQLH.validateToken(token);

    if( uid == -1)
    {
      return false;

    }


    return osmSQLH.newKeyName(sNewKeyName, nKeyNum, uid);

  } // setNewKeyName


  public boolean getKeyVisible(String sToken, int nKeyNum)
  {
    int uid = osmSQLH.validateToken(sToken);

    if( uid == -1)
    {
      return false;

    }

    return osmSQLH.getKeyVisible(nKeyNum);

  } // getKeyVisible

} // osmServerHandler
