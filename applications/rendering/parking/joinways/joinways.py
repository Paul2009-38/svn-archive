# -*- coding: utf-8 -*-
# by kay - basic functions

### config for Toolserver
#DSN = 'dbname=osm_mapnik host=sql-mapnik'
### config for devserver
#DSN = 'dbname=hstore'
### config for Crite
#DSN = 'dbname=gis host=crite'

import sys
import psycopg2
import csv
from numpy import *

"""
def shift_by_meters(lat, lon, brng, d):
    R = 6371000.0 # earth's radius in m
    lat=math.radians(lat)
    lon=math.radians(lon)
    lat2 = math.asin( math.sin(lat)*math.cos(d/R) + 
                      math.cos(lat)*math.sin(d/R)*math.cos(brng))
    lon2 = lon + math.atan2(math.sin(brng)*math.sin(d/R)*math.cos(lat), 
                             math.cos(d/R)-math.sin(lat)*math.sin(lat2))
    lat2=math.degrees(lat2)
    lon2=math.degrees(lon2)
    return [lat2,lon2]

def calc_bearing(x1,y1,x2,y2,side):
    Q = complex(x1,y1)
    R = complex(x2,y2)
    v = R-Q
    if side=='left':
        v=v*complex(0,1);
    elif side=='right':
        v=v*complex(0,-1);
    else:
        raise TypeError('side must be left or right')
    #v=v*(1/abs(v)) # normalize
    angl = angle(v) # angle (radians) (0°=right, and counterclockwise)
    bearing = math.pi/2.0-angl # (0°=up, and clockwise)
    return bearing

def unboth(lst,sideindex):
    " "" Replace in a list all rows with 'both' with two rows with 'left' and 'right'
    " ""
    list_both = list(lst)
    list_both.reverse()
    lst = []
    while len(list_both)>0:
        row = list_both.pop()
        print "row=", row
        print "row.type=", type(row)
        side = row[sideindex]
        if side=='both':
            row_l = list(row)
            row_l[sideindex] = 'left'
            #print 'bothl:', row_l
            lst += [row_l]
            row_r = list(row)
            row_r[sideindex] = 'right'
            #print 'bothr:', row_r
            lst += [row_r]
        else:
            #print side, ":", row
            lst += [row]
    return lst

if len(sys.argv) == 3:
    DSN = sys.argv[1]
    openlayertextfilename = sys.argv[2]
else:
    print "usage: osm-parking-icons.py 'dbname=osm_mapnik host=sql-mapnik' '/home/kayd/parkingicons/parkingicons.txt'"
    exit(0);

print "Opening connection using dns:", DSN
conn = psycopg2.connect(DSN)
print "Encoding for this connection is", conn.encoding
curs = conn.cursor()

openlayertextfile = csv.writer(open(openlayertextfilename, 'w'), delimiter='\t',quotechar='"', quoting=csv.QUOTE_MINIMAL)
openlayertextfile.writerow(['lat','lon','title','description','icon','iconSize','iconOffset'])

latlon= "ST_Y(ST_Transform(ST_line_interpolate_point(way,0.5),4326)),ST_X(ST_Transform(ST_line_interpolate_point(way,0.5),4326))"
coords= "ST_Y(ST_line_interpolate_point(way,0.5)) as py,ST_X(ST_line_interpolate_point(way,0.5)) as px,ST_Y(ST_line_interpolate_point(way,0.49)) as qy,ST_X(ST_line_interpolate_point(way,0.49)) as qx,ST_Y(ST_line_interpolate_point(way,0.51)) as ry,ST_X(ST_line_interpolate_point(way,0.51)) as rx"
FW = "FROM planet_line WHERE"

### display disc - maxstay

pc_disc_maxstay = []
for side in ['left','right','both']:
    curs.execute("SELECT osm_id,"+latlon+",(tags->'parking:condition:"+side+":maxstay') as \"parking:condition:"+side+":maxstay\","+coords+",'"+side+"' "+FW+" (tags ? 'parking:condition:"+side+":maxstay') and (tags ? 'parking:condition:"+side+"') and (tags->'parking:condition:"+side+"')='disc'")
    pc_disc_maxstay += curs.fetchall()

pc_disc_maxstay = unboth(pc_disc_maxstay,10)
        
for pc_dm in pc_disc_maxstay:
    side = pc_dm[10]
    bearing = calc_bearing(pc_dm[7],pc_dm[6], pc_dm[9],pc_dm[8], side)
    openlayertextfile.writerow(shift_by_meters(pc_dm[1],pc_dm[2],bearing,4.0)+['Disc parking','Maximum parking time:<br>'+pc_dm[3],'parkingicons/pi-disc.png','16,16','-8,-8'])

### display vehicles

pc_vehicles = []
for side in ['left','right','both']:
    curs.execute("SELECT osm_id,"+latlon+",(tags->'parking:condition:"+side+":vehicles') as \"parking:condition:"+side+":vehicles\","+coords+",'"+side+"' "+FW+" (tags ? 'parking:condition:"+side+":vehicles')")
    pc_vehicles += curs.fetchall()

vehicle_icons = {"car":"parkingicons/pi-car.png" , "bus":"parkingicons/pi-bus.png" , "motorcycle":"parkingicons/pi-motorcycle.png"}

pc_vehicles = unboth(pc_vehicles,10)

for pc_v in pc_vehicles:
    vehicle_icon = vehicle_icons.get(pc_v[3],"parkingicons/pi-unkn.png");
    bearing = calc_bearing(pc_v[7],pc_v[6], pc_v[9],pc_v[8], pc_v[10])    
    openlayertextfile.writerow(shift_by_meters(pc_v[1],pc_v[2],bearing,4.0)+['Parking only for','Vehicle : '+pc_v[3],vehicle_icon,'16,16','-8,-8'])

conn.rollback()

sys.exit(0)
"""

"""
SELECT
   ST_Y(way) AS lat_wgs84,
   ST_X(way) AS lon_wgs84,
   ST_X(transform(way, 31466)) AS KOORD_X,
   ST_Y(transform(way, 31466)) AS KOORD_Y,
   skeys(tags) AS key,
   svals(tags) AS value,
   osm_id
FROM
   dortmund_point
WHERE
   exist(tags, 'amenity')
LIMIT 10;

SELECT
   ST_Y(way) AS lat_wgs84,
   ST_X(way) AS lon_wgs84,
   ST_X(transform(way, 31466)) AS KOORD_X,
   ST_Y(transform(way, 31466)) AS KOORD_Y,
   key,
   value,
   tags->'name' as name,
   osm_id
FROM
   (SELECT
       osm_id, way, tags,
       (each(tags)).key,
       (each(tags)).value
    FROM
       dortmund_point
    WHERE
       exist(tags, 'amenity') /*AND exist(tags, 'name')*/
   ) AS sq
WHERE
   key = 'amenity'
LIMIT 10;

SELECT
   ST_Y(way) AS lat_wgs84,
   ST_X(way) AS lon_wgs84,
   ST_X(transform(way, 31466)) AS KOORD_X,
   ST_Y(transform(way, 31466)) AS KOORD_Y,
   key,
   value,
   tags->'name' as name,
   tags->'sport' as sport,
   osm_id
FROM
   (SELECT
       osm_id, tags,
       ST_Centroid(way) as way,
       (each(tags)).key,
       (each(tags)).value
    FROM
       dortmund_polygon
    WHERE
       tags->'natural' = 'water'
       AND
       tags->'sport' = 'swimming'
   ) AS sq
WHERE
   key = 'natural'
LIMIT 10;
"""

class OSMDB:
    DSN = None
    conn = None
    curs = None

    def __init__( self, dsn = None ):
        self.DSN = dsn
        self.conn = psycopg2.connect(self.DSN)
        print "Encoding for this connection is", self.conn.encoding
        self.curs = self.conn.cursor()
        
def main_approach(options):
    bbox = options['bbox']
    DSN = options['dsn']
    print bbox
    osmdb = OSMDB(DSN)
    #highways = getHighwaysInBbox(DSN,bbox)

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-b", "--bbox", dest="bbox", help="bounding box to restrict to", default="")
    parser.add_option("-d", "--dsn", dest="dsn", help="DSN, default is 'dbname=gis host=crite'", default="dbname=gis host=crite")
    (options, args) = parser.parse_args()
    print options
    main(options.__dict__)
    sys.exit(0)
