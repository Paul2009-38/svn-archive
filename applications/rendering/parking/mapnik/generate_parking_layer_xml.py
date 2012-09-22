# -*- coding: utf-8 -*-
# by kay

import sys,os,subprocess,shutil
from optparse import OptionParser
#from xml.dom.minidom import parse, parseString
import pxdom

#import colorsys

condition_colors = {
    'free': '7fff00',
    'disc': '50a100',
    'cust': 'b68529',
    'resi': '785534',
    'priv': '3f2920',
    'fee':  '67a1eb',
    'unkn': 'bc73e2' # purple:'bc73e2' ; bluegreen:'6fc58a' 
    }

forbidden_colors = {
    'nopa': 'f8b81f',
    'nost': 'f85b1f',
    'fire': 'f81f1f'
    }

def transmogrify_file(sf,dfgrey,dfnoicons):
    dom= pxdom.getDOMImplementation('') 
    parser= dom.createLSParser(dom.MODE_SYNCHRONOUS, None)
    parser.domConfig.setParameter('entities', 0) # 1 -> exception if attribute values is set
    #parser.domConfig.setParameter('disallow-doctype', 1)
    parser.domConfig.setParameter('pxdom-resolve-resources', 1) # 1 -> replace &xyz; with text
    document = parser.parseURI(sf)

#    dom_convert_to_grey(document)
    
    output= document.implementation.createLSOutput() 
    output.systemId= dfgrey
    output.encoding= 'utf-8' 
    serialiser= document.implementation.createLSSerializer() 
    serialiser.write(document, output)

def strip_doctype(f):
    p = subprocess.Popen(['sed','-i','2,10 d',f]) # -i means 'in place'
    p.wait()

def create_parking_icons(source_symbols_dir,dest_symbols_dir):
    create_parking_lane_icons(source_symbols_dir,dest_symbols_dir)
    create_parking_area_icons(source_symbols_dir,dest_symbols_dir)
    create_parking_point_icons(source_symbols_dir,dest_symbols_dir)
    
def create_parking_lane_icons(source_symbols_dir,dest_symbols_dir):
    # first create mirror images (from left to right)
    image_files = os.listdir(source_symbols_dir)
    image_files = [f for f in image_files if f.startswith('park-l') and f.endswith('png')]
    print image_files
    for f in image_files:
        sf = os.path.join(source_symbols_dir,f)
        df = os.path.join(source_symbols_dir,f) # this is changed so that we write in the source dir
        hflip_icon(sf,df.replace('-l','-r'))

    # then, create the colors
    image_files = os.listdir(source_symbols_dir)
    image_files = [f for f in image_files if f.endswith('source.png')]
    for f in image_files:
        # convert ./original-mapnik/symbols/*.png -fx '0.25*r + 0.62*g + 0.13*b' ./bw-mapnik/symbols/*.png
        sf = os.path.join(source_symbols_dir,f)
        df = os.path.join(dest_symbols_dir,f)
        if 'n-' in f:      # then it's a non-parking thing
            for c in forbidden_colors.iterkeys():
                colorize_icon(sf,df.replace('source',c),forbidden_colors.get(c))
        else:
            for c in condition_colors.iterkeys():
                colorize_icon(sf,df.replace('source',c),condition_colors.get(c))

def create_parking_area_icons(source_symbols_dir,dest_symbols_dir):
    tempf = "/tmp/2347856893476512873465.png"
    for icon_name_prefix  in ["parking_area","parking_multistorey","parking_underground"]:
        df = os.path.join(dest_symbols_dir,icon_name_prefix+"_source.png")
        stampf = os.path.join(source_symbols_dir,icon_name_prefix+"_stamp.png")
        for c in condition_colors.iterkeys():
            # step 1: colorize a "stamp" template image
            p = subprocess.Popen(['convert','-size','16x16','xc:#'+condition_colors.get(c),stampf,'-compose','Darken','-composite',tempf])
            p.wait()
            # step 2: make it 50% transparent
            # convert -size 16x16 parking_area_free.png xc:grey -alpha off -compose Copy_Opacity -composite  /tmp/a.png && gwenview /tmp/a.png
            p = subprocess.Popen(['convert','-size','16x16',tempf,'xc:gray','-alpha','off','-compose','Copy_Opacity','-composite',df.replace('source',c)])
            p.wait()


def create_parking_point_icons(source_symbols_dir,dest_symbols_dir):
    tempf = "/tmp/2347856893476512873465.png"
    stampf = os.path.join(source_symbols_dir,"parking_node_stamp.png")
    # for now there's only the parking-vending icon
    copy_files(source_symbols_dir,dest_symbols_dir,['parking-vending.png'])
    # parking nodes
    for condition in condition_colors.keys():
        # convert ./original-mapnik/symbols/*.png -fx '0.25*r + 0.62*g + 0.13*b' ./bw-mapnik/symbols/*.png
        sf = os.path.join(source_symbols_dir,'parking_node_source.png')
        df = os.path.join(dest_symbols_dir,'parking_node_{cond}.png'.format(cond=condition))
        colorize_icon(sf,tempf,condition_colors.get(condition))
        p = subprocess.Popen(['convert','-size','16x16',tempf,stampf,'-compose','Darken','-composite',df])
        print (['convert','-size','16x16',tempf,stampf,'-compose','Darken','-composite',df])
        p.wait()

def copy_files(src,dest,files):
    for f in files:
        if type(f) is tuple:
            shutil.copy2(os.path.join(src,f[0]),os.path.join(dest,f[1]))
        else:
            shutil.copy2(os.path.join(src,f),os.path.join(dest,f))

def colorize_icon(sf,df,color):
    p = subprocess.Popen(['convert',sf,'-fill','#'+color,'-colorize','100',df])
    p.wait()

def hflip_icon(sf,df):
    p = subprocess.Popen(['convert',sf,'-flip',df])
    p.wait()

def stamp_icon(sf,df,stampf):
    p = subprocess.Popen(['convert',sf,stampf,'-compose','Darken','-composite',df])
    p.wait()

def add_license_files(dirname):
    f = open(os.path.join(dirname,"CONTACT"), 'w')
    f.write("This style is created by kayd@toolserver.org")
    f.close
    f = open(os.path.join(dirname,"LICENSE"), 'w')
    f.write("This derived work is published by the author, Kay Drangmeister, under the same license as the original OSM mapnik style sheet (found here: http://svn.openstreetmap.org/applications/rendering/mapnik)")
    f.close

def main_parktrans(options):
    style_name = options['stylename']
    source_dir = options['sourcedir']
    source_file = options['sourcefile']
    #source_symbols_dir_mapnik = os.path.join(source_dir,"symbols")
    source_symbols_dir_style = os.path.join(source_dir,"parking-symbols-src")
    dest_dir = options['destdir']

    # parktrans - transparent layer to be put on top of mapnik or bw-noicons base layer
    
    dest_dir_style = os.path.join(dest_dir,style_name)
    dest_dir_style_symbols = os.path.join(dest_dir_style,"symbols")
    dest_file_style = 'osm-{style}.xml'.format(style=style_name)
    style_file = os.path.join(dest_dir_style,dest_file_style)
    if not os.path.exists(dest_dir_style_symbols):
        os.makedirs(dest_dir_style_symbols)

    create_parking_icons(source_symbols_dir_style,dest_dir_style_symbols)
    transmogrify_file(os.path.join(source_dir,source_file),style_file,"")
    strip_doctype(style_file)
    add_license_files(dest_dir_style)

def main_parking(options):
    style_name = options['stylename']
    source_bwn_dir = options['sourcebwndir']
    source_bwn_file = options['sourcebwnfile']
    source_p_dir = options['sourcepdir']
    source_p_file = options['sourcepfile']
    source_symbols_dir_style = os.path.join(source_p_dir,"parking-symbols-src")
    dest_dir = options['destdir']

    dest_dir_style = os.path.join(dest_dir,style_name)
    dest_dir_style_symbols = os.path.join(dest_dir_style,"symbols")
    dest_file_style = 'osm-{style}.xml'.format(style=style_name)
    style_file = os.path.join(dest_dir_style,dest_file_style)
    if not os.path.exists(dest_dir_style_symbols):
        os.makedirs(dest_dir_style_symbols)

    create_parking_icons(source_symbols_dir_style,dest_dir_style_symbols)
    merge_bw_noicons_and_parktrans_style(os.path.join(source_bwn_dir,source_bwn_file),os.path.join(source_p_dir,source_p_file),style_file)
    #strip_doctype(style_file)
    add_license_files(dest_dir_style)

def merge_bw_noicons_and_parktrans_style(bwnoicons_style_file,parktrans_style_file,dest_parking_style_file):
    dom= pxdom.getDOMImplementation('') 
    parser= dom.createLSParser(dom.MODE_SYNCHRONOUS, None)
    parser.domConfig.setParameter('entities', 0) # 1 -> exception if attribute values is set
    #parser.domConfig.setParameter('disallow-doctype', 1)
    parser.domConfig.setParameter('pxdom-resolve-resources', 1) # 1 -> replace &xyz; with text
    dest_parking_style_document = parser.parseURI(bwnoicons_style_file)
    parktrans_style_document = parser.parseURI(parktrans_style_file)

    parking_area_style = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parking-area'))
    parking_area_layer = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parking-area'))
    things=[parking_area_style,parking_area_layer]
    #better put parking area layer earlier, before all roads 
    parking_dom_insert_things_before_layer(dest_parking_style_document,things,'turning_circle-casing')
    # duplicate the parking-area layer in order to make it less transparent
    clone_of_parking_area_layer = parking_dom_clone_layer(dest_parking_style_document,'parking-area')
    clone_of_parking_area_layer.setAttribute('name','parking-area-double')
    parking_dom_insert_things_before_layer(dest_parking_style_document,clone_of_parking_area_layer,'turning_circle-casing')

    #add a second parking area layer on top of the parking-aisle roads.
    parking_area_top_layer = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parking-area-top'))
    things=[parking_area_top_layer]
    parking_dom_insert_things_before_layer(dest_parking_style_document,things,'direction_pre_bridges')

    # handle the parking lanes
    pllno = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-left-no'))
    plrno = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-right-no'))
    pllin = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-left-parallel'))
    plrin = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-right-parallel'))
    plldi = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-left-diagonal'))
    plrdi = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-right-diagonal'))
    pllor = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-left-perpendicular'))
    plror = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-right-perpendicular'))
    pllma = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-left-marked'))
    plrma = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-right-marked'))
    pll = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parkinglane-left'))
    plr = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parkinglane-right'))
    things=[pllno,plrno,pllin,plrin,plldi,plrdi,pllor,plror,pllma,plrma,pll,plr]
    if True: # add maxstay styles and layers
        plmls = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-maxstay-left'))
        plmll = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parkinglane-maxstay-left'))
        plmrs = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parkinglane-maxstay-right'))
        plmrl = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parkinglane-maxstay-right'))
        things+=[plmls,plmll,plmrs,plmrl]
    parking_dom_insert_things_before_layer(dest_parking_style_document,things,'admin-01234')

    # handle the parking points / nodes
    parking_points_style = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parking-points'))
    parking_points_layer = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parking-points'))
    parking_area_text_style = dest_parking_style_document.adoptNode(parking_dom_cut_style(parktrans_style_document,'parking-area-text'))
    parking_area_text_layer = dest_parking_style_document.adoptNode(parking_dom_cut_layer(parktrans_style_document,'parking-area-text'))
    things=[parking_points_style,parking_points_layer,parking_area_text_style,parking_area_text_layer]
    parking_dom_insert_things_before_layer(dest_parking_style_document,things,'admin-01234')

    output= dest_parking_style_document.implementation.createLSOutput() 
    output.systemId= dest_parking_style_file
    output.encoding= 'utf-8' 
    serialiser= dest_parking_style_document.implementation.createLSSerializer() 
    serialiser.write(dest_parking_style_document, output)

    ''' write a "rest" file to check if everything has been cut out
    output= parktrans_style_document.implementation.createLSOutput() 
    output.systemId= 'rest.xml'
    output.encoding= 'utf-8' 
    serialiser= parktrans_style_document.implementation.createLSSerializer() 
    serialiser.write(parktrans_style_document, output)
    '''

def parking_dom_insert_things_before_layer(document,things,here):
    # insert things after the "leisure" layer
    els = document.getElementsByTagName("Layer")
    #print "els="
    #print els
    for el in els:
        #print "layername={ln}".format(ln=el.getAttribute("name"))
        if el.getAttribute("name")==here:
            #print "found it"
            if type(things) is list:
                for s in things:
                    el.parentNode.insertBefore(s,el)
            else:
                el.parentNode.insertBefore(things,el)
            return
    raise 'Layer name not found'

def parking_dom_clone_layer(document,what):
    els = document.getElementsByTagName("Layer")
    for el in els:
        #print "layername={ln}".format(ln=el.getAttribute("name"))
        if el.getAttribute("name")==what:
            #print "found it"
            clone = el.cloneNode(True)
            return clone
    raise BaseException('Layer name {ln} not found'.format(ln=what))

def parking_dom_cut_layer(document,what):
    els = document.getElementsByTagName("Layer")
    for el in els:
        #print "layername={ln}".format(ln=el.getAttribute("name"))
        if el.getAttribute("name")==what:
            #print "found it"
            el.parentNode.removeChild(el)
            return el
    raise BaseException('Layer name {ln} not found'.format(ln=what))

def parking_dom_cut_style(document,what):
    els = document.getElementsByTagName("Style")
    for el in els:
        #print "layername={ln}".format(ln=el.getAttribute("name"))
        if el.getAttribute("name")==what:
            #print "found it"
            el.parentNode.removeChild(el)
            return el
    raise BaseException('Style name {sn} not found'.format(sn=what))

"""
./generate_xml.py osm-parking-src.xml    osm-parking.xml    --accept-none --host sql-mapnik --dbname osm_mapnik --prefix planet --inc ./parking-inc --symbols ./parking-symbols/ --world_boundaries /home/project/o/s/m/osm/data/world_boundaries/ --password ''
./generate_xml.py osm-parking-bw-src.xml osm-parking-bw.xml --accept-none --host sql-mapnik --dbname osm_mapnik --prefix planet --inc ./parking-inc --symbols ./parking-symbols/ --world_boundaries /home/project/o/s/m/osm/data/world_boundaries/ --password ''
./generate_xml.py osm-parkerr-src.xml    osm-parkerr.xml    --accept-none --host sql-mapnik --dbname osm_mapnik --prefix planet --inc ./parking-inc --symbols ./parking-symbols/ --world_boundaries /home/project/o/s/m/osm/data/world_boundaries/ --password ''
"""

if __name__ == '__main__':
    parser = OptionParser()
    parser.add_option("-s", "--sourcedir", dest="sourcedir", help="path to the source directory", default=".")
    parser.add_option("-f", "--sourcefile", dest="sourcefile", help="source filename, default is 'osm.xml')", default="osm.xml")
    parser.add_option("-d", "--destdir", dest="destdir", help="path to the destination directory, further dirs are created within. default is '/tmp'", default="/tmp")
    (options, args) = parser.parse_args()
    options['stylename'] = "parktrans"
    print options
    main_parktrans(options.__dict__)
    options['sourcefile'] = "osm-parking-src.xml"
    options['stylename'] = "parking"
    main_parking(options.__dict__)
    sys.exit(0)
