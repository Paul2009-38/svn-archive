#!/usr/bin/python
#-----------------------------------------------------------------------------# Map tile server
#
# Features:
#   * Serves slippy-map tile images
#   * Uses OsmRender module to render them
#-----------------------------------------------------------------------------# Copyright 2008, Oliver White
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#----------------------------------------------------------------------------
from BaseHTTPServer import *
import OsmRender
import re
import sys

class tileServer(BaseHTTPRequestHandler):
  def __init__(self, request, client_address, server):
    self.re = re.compile('/(\w+)/(\d+)/(\d+)/(\d+)\.png')
    BaseHTTPRequestHandler.__init__(self, request, client_address, server)

  def log_message(self, format, *args):
    pass  # Kill logging to stderr
  
  def do_GET(self):
    # See if a tile was requested
    match = self.re.search(self.path)
    if(match):
      (layer,z,x,y) = match.groups()
      z = int(z)
      x = int(x)
      y = int(y)
      
      
      # Render the tile
      print 'Request for %d,%d at zoom %d, layer %s' % (x,y,z,layer)
      pngData = OsmRender.RenderTile(z,x,y, None)
      
      if(pngData == None):
        print "Not found"
        self.send_response(404)
        return
      
      # Return the tile as a PNG
      self.send_response(200)
      self.send_header('Content-type','image/PNG')
      self.end_headers()
      self.wfile.write(pngData)
    else:
      self.send_response(404)

try:
  server = HTTPServer(('',1280), tileServer)
  server.serve_forever()
except KeyboardInterrupt:
  server.socket.close()
  sys.exit()

