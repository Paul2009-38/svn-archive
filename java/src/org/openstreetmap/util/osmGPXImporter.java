
package org.openstreetmap.util;

import java.util.*;
import java.io.*;
import javax.xml.parsers.*;

import org.xml.sax.helpers.*;
import org.xml.sax.*;

import org.apache.xmlrpc.*;
//import org.apache.xerces.parsers.SAXParser;

public class osmGPXImporter extends DefaultHandler{

  private PrintWriter out = null;
 
  private double lat = 0;
  private double lon = 0;
  private double ele = 0;
  private int fix = 0;
  
  private long timestamp = 0;

  private static final int MODE_GET_TIME = 1;
  private static final int MODE_GET_ELE = 2;

  private int nMode = -1; 

  private boolean bLatLonValid = false;
  private boolean bEleValid = false;
  private boolean bTimeValid = false;

  String sCurrent = "";
  boolean bDoneGetting = false;

  XmlRpcClientLite xmlrpc;
  String sToken = "";

  String sUser;
  String sPass;

  public osmGPXImporter()
  {

  }
  
  public osmGPXImporter(PrintWriter o, String u, String p)
  {
    out = o;

    connectToServer(u,p);


  } // osmGPXImporter

  
  public void startElement(
      String namespaceURI,
      String localName,
      String qName,
      Attributes atts)
  {
    
     
    if( qName.equals("trkpt"))    
    {
    
      lat = Double.parseDouble(atts.getValue("lat"));
      lon = Double.parseDouble(atts.getValue("lon"));

      bLatLonValid = true;
    
    }   
 
    if( qName.equals("time"))
    {
    
      sCurrent = "";
        
    }

    if( qName.equals("ele"))
    {
    
      sCurrent = "";
        
    }
       
  
  
  } // startElement



  public void characters(char[] ch,
                       int start,
                       int length)
  {

    String s = new String(ch, start, length);

    sCurrent += s;
    
  } // characters


  private void setDate(String s)
  {

    StringTokenizer t = new StringTokenizer(s.trim(), " -:TZ");


    int year = Integer.parseInt(t.nextToken()) - 1900;
    
    int month = Integer.parseInt(t.nextToken()) - 1;
    int day = Integer.parseInt(t.nextToken()) - 1;
    int hour = Integer.parseInt(t.nextToken());
    int min = Integer.parseInt(t.nextToken());
    int sec = Integer.parseInt(t.nextToken());
      
    timestamp = new Date(year,month,day,hour,min,sec).getTime();
  } // setDate


  private void setEle(String s)
  {
    ele = Double.parseDouble(s.trim());

  } // setEle


  private void setFix(String s)
  {
    s = s.trim();
    
  
    if( s.equals("2d"))
    {
      fix = 3;

    }


    if( s.equals("3d"))
    {
      fix = 4;

    }


    if( s.equals("none"))
    {
      fix = 0;

    }

    

  } // setFix

  
  public void endElement(
      String uri,
      String localName,
      String qName
      )
  {

    if( qName.equals("trkpt"))
    {
      addPoint();

    }

    
 
    if( qName.equals("ele"))
    {
      setEle(sCurrent);

    }
    if( qName.equals("time"))
    {
      setDate(sCurrent);

    }
    
  } // endElement


  private void connectToServer(String sUser, String sPass)
  {

    try
    {
      xmlrpc = new XmlRpcClientLite("http://www.openstreetmap.org/api/xml.jsp");
 
      Vector v = new Vector();

      v.addElement(sUser);
      v.addElement(sPass);
     
      sToken = (String)xmlrpc.execute("openstreetmap.login", v);

      out.print("logged in with token " + sToken);
    
      
    }
    catch(Exception e)
    {
      out.println(e);
    }
    
  } // connectToServer

  
  private void addPoint()
  {
    try{

      Thread.sleep(10);

    }
    catch(Exception e)
    {

    }
        

   
    Vector v = new Vector();

      v.addElement(sToken);
      v.addElement(new Double(lat)); // lat
      v.addElement(new Double(lon)); // lon
      v.addElement(new Double(ele)); // alt
      v.addElement( new Date(timestamp) ); // timestamp for point
      v.addElement(new Double(-1)); // hor_dilution
      v.addElement(new Double(-1)); // vert_dilution
      v.addElement(new Integer(-1)); // track_id
      v.addElement(new Integer(255)); // quality
      v.addElement(new Integer(fix)); // satellites
      
      
      boolean b = false;
        
      try{
        
        
        b = ((Boolean)xmlrpc.execute("openstreetmap.addPoint", v)).booleanValue();
      }
      catch(Exception e)
      {
        b = false;

        out.println(e);

      }

        

      if( !b )
      {
      
        out.println("failed to add point " + lat + "," + lon + "," + ele + ": " + fix + " @" + new Date(timestamp));

        out.println("this is bad, quiting");

      }
      else
      {
        out.print(".");

      }
  } // addPoint

  

  
  public void upload(BufferedInputStream is, Writer out, String user, String pass)
  {
    PrintWriter o = new PrintWriter(out);

    try{
    
      osmGPXImporter handler = new osmGPXImporter(o, user, pass);


      SAXParserFactory factory = SAXParserFactory.newInstance();

      SAXParser saxParser = factory.newSAXParser();

      saxParser.parse( is, handler );
    }
    catch(Exception e)
    {
      o.print(e);
    }

  }
}

