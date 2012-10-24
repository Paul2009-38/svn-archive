# -*- coding: utf-8 -*-
# by kay

import sys,string
import psycopg2
from geom import bbox

class OSMDB:
    """ Handles queries to the planet database """

    def __init__(self,dsn,prefix="planet"):
        self.DSN = dsn
        self.prefix = prefix
        self.LIMIT = ""

        print "Opening connection using dsn:", self.DSN
        self.conn = psycopg2.connect(self.DSN)
        print "Encoding for this connection is", self.conn.encoding
        self.curs = self.conn.cursor()

        self.clear_globalboundingbox()

        self.latlon= 'ST_Y(ST_Transform(ST_line_interpolate_point(way,0.5),4326)),ST_X(ST_Transform(ST_line_interpolate_point(way,0.5),4326))'
        self.coords= "ST_Y(ST_line_interpolate_point(way,0.5)) as py,ST_X(ST_line_interpolate_point(way,0.5)) as px,ST_Y(ST_line_interpolate_point(way,0.49)) as qy,ST_X(ST_line_interpolate_point(way,0.49)) as qx,ST_Y(ST_line_interpolate_point(way,0.51)) as ry,ST_X(ST_line_interpolate_point(way,0.51)) as rx"
        self.FlW = "FROM "+prefix+"_line WHERE"
        self.FpW = "FROM "+prefix+"_polygon WHERE"
        self.FnW = "FROM "+prefix+"_point WHERE"

    def __del__(self):
        print "Closing connection"
        self.conn.commit()
        self.conn.close()

    def commit(self):
        self.conn.commit()

    def select(self,select):
        self.curs.execute(select)
        rs = self.curs.fetchall()
        return rs

    def select_one(self,select):
        """ Return exactly one result from select. None if no result rows. """
        self.curs.execute(select)
        rs = self.curs.fetchall()
        if len(rs)==0:
            return None
        return rs[0][0]

    def select_list(self,select):
        """ Return a list of results (one column only!) from select. Empty list if no result rows. """
        self.curs.execute(select)
        rs = self.curs.fetchall()
        l = []
        for res in rs:
            l.append(res[0])
        return l

    def insert(self,insert):
        self.curs.execute(insert)

    def update(self,update):
        self.curs.execute(update)

    def delete(self,delete):
        self.curs.execute(delete)

    def sql_list_of_ids(self,liste):
        """ Returns string: list of IDs, e.g. '(1,2,3)' from [1,2,3] for use in sql queries"""
        list_ids_as_strings=map(lambda osmid: str(osmid),liste)
        return "("+string.join(list_ids_as_strings,',')+")"

    def _escape_quote(self,name):
        return name.replace("'","''")

    def _quote_or_null(self,text):
        """ for update statements: escape names with single quotes and surround them with single quotes, or 'Null' if text is None """
        if text==None:
            return 'Null'
        return "'"+text.replace("'","''")+"'"


# ---------------------------------------------------------------------------
# old
    def clear_globalboundingbox(self):
        """ clears the global bounding box """
        self.globalboundingbox = None

    def set_globalboundingbox(self,bbox,srs = '4326'):
        """ sets the global bounding box """
        self.globalboundingbox = bbox({'bbox':bbox,'srs':srs})

    def get_globalboundingbox(self,bbox,srs = '4326'):
        """ gets the global bounding box """
        return self.globalboundingbox

    """
    def clear_bbox(self):
        self.bbox = None
        self.googbox = None
        
    def set_bbox(self,bbox,srs = '4326'):
        #options.get('srs','4326')
        if srs=='4326':
            self.init_bbox_srs(bbox, '4326')
        elif srs=='3857':
            self.init_bbox_srs(bbox, '3857')
        elif srs=='900913':
            self.init_bbox_srs(bbox, '3857')
        else:
            raise ValueError("Unknown srs "+str(srs))
    
    def init_bbox_srs(self,bbox,srs):
        self.googbox = "transform(SetSRID('BOX3D("+bbox+")'::box3d,"+srs+"),900913)"
        self.curs.execute("SELECT ST_AsText("+self.googbox+") AS geom")
        self.bbox = self.curs.fetchall()
        self.get_bounds()

    def get_bounds(self):
        polygonstring = self.bbox[0][0]
        polygonstring = polygonstring[9:] # cut off the "POLYGON(("
        polygonstring = polygonstring[:-2] # cut off the "))"
        points = polygonstring.split(',')

        numpoints = len(points)
        for i,point in enumerate(points):
            latlon = point.split(' ')
            if (i==0):
                self.left=float(latlon[0])
                self.bottom=float(latlon[1])
            if (i==2):
                self.right=float(latlon[0])
                self.top=float(latlon[1])
        print "Bounds [b l t r] = ",self.bottom,self.left,self.top,self.right

    def coords_from_bbox(self,bbox):
        bbox = bbox[4:] # cut off the "BOX("
        bbox = bbox[:-1] # cut off the ")"
        
        bbox = bbox.replace(' ',',')
        coordslist = map(lambda coord: float(coord), bbox.split(','))
        return tuple(coordslist)
    """

# ---------------------------------------------------------------------------
    """
    def select_highways(self):
        self.curs.execute("SELECT osm_id,highway,ST_AsText(\"way\") AS geom, tags->'lanes' as lanes, tags->'layer' as layer, tags->'oneway' as oneway, tags->'lanes:forward' as lanesfw, tags->'lanes:forward' as lanesbw "+self.FlW+" \"way\" && "+self.googbox+" and highway is not NULL "+self.LIMIT+";")
        rs = self.curs.fetchall()
        highways = []
        for res in rs:
            highway = {}
            highway['osm_id']=res[0]
            highway['highway']=res[1]
            highway['coords']=WKT_to_line(res[2])
            highway['lanes']=res[3]
            highway['layer']=res[4]
            highway['oneway']=res[5]
            highway['lanesfw']=res[6]
            highway['lanesbw']=res[7]
            highways.append(highway)
        return highways

    def select_highway_areas(self):
        self.curs.execute("SELECT osm_id,highway,ST_AsText(\"way\") AS geom, tags->'height' as height, amenity, ST_AsText(buffer(\"way\",1)) AS geombuffer  "+self.FpW+" \"way\" && "+self.googbox+" and highway is not NULL "+self.LIMIT+";")
        rs = self.curs.fetchall()
        areas = []
        for res in rs:
            area = {}
            area['osm_id']=res[0]
            area['highway']=res[1]
            area['coords']=WKT_to_polygon(res[2])
            area['height']=res[3]
            area['amenity']=res[4]
            area['buffercoords']=WKT_to_polygon(res[5])
            areas.append(area)
        return areas

    def select_amenity_areas(self):
        self.curs.execute("SELECT osm_id,amenity,ST_AsText(\"way\") AS geom, ST_AsText(buffer(\"way\",1)) AS geombuffer, tags->'height' as height  "+self.FpW+" \"way\" && "+self.googbox+" and amenity is not NULL and (building is NULL or building='no')"+self.LIMIT+";")
        rs = self.curs.fetchall()
        areas = []
        for res in rs:
            area = {}
            area['osm_id']=res[0]
            area['amenity']=res[1]
            area['coords']=WKT_to_polygon(res[2])
            area['buffercoords']=WKT_to_polygon(res[3])
            area['height']=res[4]
            areas.append(area)
        return areas

    def select_buildings(self,buildingtype):
        self.curs.execute("SELECT osm_id,ST_AsText(\"way\") AS geom, building, tags->'height' as height,tags->'building:height' as bheight,amenity,shop "+self.FpW+" \"way\" && "+self.googbox+" and building='"+buildingtype+"' "+self.LIMIT+";")
        rs = self.curs.fetchall()
        buildings = []
        for res in rs:
            building = {}
            building['osm_id']=res[0]
            building['coords']=WKT_to_polygon(res[1])
            building['building']=res[2]
            building['height']=res[3]
            building['bheight']=res[4]
            building['amenity']=res[5]
            building['shop']=res[6]
            buildings.append(building)
        return buildings

    def select_landuse(self,landusetype):
        #print "SELECT osm_id,landuse,ST_AsText(\"way\") AS geom "+self.FpW+" \"way\" && "+self.googbox+" and landuse='"+landusetype+"' "+LIMIT+";"
        self.curs.execute("SELECT osm_id,landuse,ST_AsText(\"way\") AS geom "+self.FpW+" \"way\" && "+self.googbox+" and landuse='"+landusetype+"' "+self.LIMIT+";")
        rs = self.curs.fetchall()
        landuses = []
        for res in rs:
            landuse = {}
            landuse['osm_id']=res[0]
            landuse['landuse']=res[1]
            landuse['coords']=WKT_to_polygon(res[2])
            landuses.append(landuse)
        return landuses

    def select_landuse_areas(self):
        self.curs.execute("SELECT osm_id,landuse,ST_AsText(\"way\") AS geom "+self.FpW+" \"way\" && "+self.googbox+" and landuse is not NULL "+self.LIMIT+";")
        rs = self.curs.fetchall()
        landuses = []
        for res in rs:
            landuse = {}
            landuse['osm_id']=res[0]
            landuse['landuse']=res[1]
            landuse['coords']=WKT_to_polygon(res[2])
            landuses.append(landuse)
        return landuses

    def select_leisure_areas(self):
        self.curs.execute("SELECT osm_id,leisure,ST_AsText(\"way\") AS geom "+self.FpW+" \"way\" && "+self.googbox+" and leisure is not NULL "+self.LIMIT+";")
        rs = self.curs.fetchall()
        leisures = []
        for res in rs:
            leisure = {}
            leisure['osm_id']=res[0]
            leisure['leisure']=res[1]
            leisure['coords']=WKT_to_polygon(res[2])
            leisures.append(leisure)
        return leisures

    def select_waterway(self,waterwaytype):
        self.curs.execute("SELECT osm_id,waterway,ST_AsText(\"way\") AS geom "+self.FpW+" \"way\" && "+self.googbox+" and waterway='"+waterwaytype+"' "+self.LIMIT+";")
        rs = self.curs.fetchall()
        waterways = []
        for res in rs:
            waterway = {}
            waterway['osm_id']=res[0]
            waterway['waterway']=res[1]
            waterway['coords']=WKT_to_polygon(res[2])
            waterways.append(waterway)
        return waterways

    def select_naturalwater(self):
        naturaltype='water'
        self.curs.execute("SELECT osm_id,tags->'natural' as natural,ST_AsText(\"way\") AS geom, tags->'type' as type, layer "+self.FpW+" \"way\" && "+self.googbox+" and tags->'natural'='"+naturaltype+"' "+self.LIMIT+";")
        rs = self.curs.fetchall()
        waters = []
        for res in rs:
            water = {}
            water['osm_id']=res[0]
            water['natural']=res[1]
            water['coords']=WKT_to_polygon(res[2])
            water['type']=res[3]
            water['layer']=res[4]
            waters.append(water)
        return waters

    def select_trees(self):
        naturaltype='tree'
        self.curs.execute("SELECT osm_id,tags->'natural' as natural,ST_AsText(\"way\") AS geom, tags->'type' as type, tags->'height' as height "+self.FnW+" \"way\" && "+self.googbox+" and tags->'natural'='"+naturaltype+"' "+self.LIMIT+";")
        rs = self.curs.fetchall()
        trees = []
        for res in rs:
            tree = {}
            tree['osm_id']=res[0]
            tree['natural']=res[1]
            tree['coords']=WKT_to_point(res[2])
            tree['type']=res[3]
            tree['height']=res[4]
            trees.append(tree)
        return trees

    def select_barriers(self):
        #print "barriers: SELECT osm_id,tags->'barrier' as barrier,ST_AsText(\"way\") AS geom, tags->'height' as height "+self.FnW+" \"way\" && "+self.googbox+" and tags ? 'barrier' "+LIMIT+";"
        self.curs.execute("SELECT osm_id,tags->'barrier' as barrier,ST_AsText(\"way\") AS geom, tags->'height' as height "+self.FnW+" \"way\" && "+self.googbox+" and tags ? 'barrier' "+self.LIMIT+";")
        rs = self.curs.fetchall()
        barriers = []
        for res in rs:
            barrier = {}
            barrier['osm_id']=res[0]
            barrier['barrier']=res[1]
            barrier['coords']=WKT_to_point(res[2])
            barrier['height']=res[3]
            barriers.append(barrier)
        return barriers

    def select_barrier_lines(self):
        self.curs.execute("SELECT osm_id,tags->'barrier' as barrier,ST_AsText(ST_Buffer(\"way\",0.35)) AS geom, tags->'height' as height "+self.FlW+" \"way\" && "+self.googbox+" and tags ? 'barrier' "+self.LIMIT+";")
        # ,'join=mitre mitre_limit=5.0' (requires GEOS 3.2)
        rs = self.curs.fetchall()
        barriers = []
        for res in rs:
            barrier = {}
            barrier['osm_id']=res[0]
            barrier['barrier']=res[1]
            barrier['coords']=WKT_to_polygon(res[2])
            barrier['height']=res[3]
            barriers.append(barrier)
        return barriers
    """
