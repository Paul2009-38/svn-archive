#!/usr/bin/env python

import os, sys, logging, zipfile, random, re, stat, signal
# we need to insert the basedir to the python path (strip 2 path components) if we want to directly execute this file
sys.path.insert(0, os.path.dirname(os.path.dirname(sys.path[0])))
os.environ['DJANGO_SETTINGS_MODULE'] = "tah.settings"
import shutil
from datetime import datetime
from time import sleep,clock,time
from django.conf import settings
from tah.tah_intern.models import Settings, Layer
from tah.tah_intern.Tileset import Tileset
from tah.tah_intern.Tile import Tile
from tah.requests.models import Request,Upload

### TileUpload returns 0 on success and >0 otherwise
class TileUpload:
  unzip_path=None #path to be used for unzipping (temporary files)
  base_tilepath=None # base tile directory
  fname=None      #complete path of uploaded tileset file
  uid=None        #random unique id which is used for pathnames
  upload=None     #current handled upload object
  tmptiledir=None #usually unzip_path+uid contains the unzipped files

  def __init__(self,config):
    self.unzip_path = config.getSetting(name='unzipPath')
    self.base_tilepath = settings.TILES_ROOT
    if self.unzip_path == None or self.base_tilepath == None:
      sys.exit("Failed to get required settings.")

  def process(self):
   try:
    while True:
      #find the oldest unlocked upload file
      self.upload = None
      while not self. upload:
        # repeat fetching until there is one
        try:
          self.upload = Upload.objects.filter(is_locked=False)[0]
        except IndexError:
          #logging.debug('No uploaded request. Sleeping 10 sec.')
          sleep(10)
      starttime = (time(),clock()) # start timing tileset handling now
      self.fname = self.upload.get_file_filename()
      if os.path.isfile(self.fname):
        #logging.debug('Handling next tileset: ' + self.upload.file)
        self.uid = str(random.randint(0,9999999999999999999))
        if self.unzip():
          tset = self.movetiles()
          if tset.layer and tset.base_z != None and tset.x != None and tset.y !=None:
            #It's a valid tileset. Save the tileset at it's place
            time_save = [time()]
            logging.debug("Saving tileset at (%s,%d,%d,%d) from user %s (uuid %d)" % (tset.layer,tset.base_z,tset.x,tset.y,self.upload.user_id,self.upload.client_uuid))
            (retval,unknown_tiles) = tset.save(self.base_tilepath, self.upload.user_id.id)
            time_save.append(time())
            if retval:
              # everything went fine. Add to user statistics
              self.add_user_stats(1365-unknown_tiles)
              # now match up the upload with a request and mark the request as finished
              reqs = Request.objects.filter(min_z = tset.base_z, x = tset.x ,y = tset.y, status__lt=2)
              for req in reqs:
                # remove corresponding layer from request and set it to status=2 when all layers are done
                req.layers.remove(tset.layer)
                if req.layers.count() == 0:
                  req.status=2
                  req.clientping_time=datetime.now()
                  req.save()
              logging.debug('Finished "%s,%d,%d,%d" in %.1f sec (CPU %.1f). Saving took %.1f sec. %d unknown tiles.' % (tset.layer,tset.base_z,tset.x,tset.y,time()-starttime[0],clock()-starttime[1], time_save[1] - time_save[0], unknown_tiles))
            else:
              # saving the tileset went wrong
              logging.error('Saving tileset "%s,%d,%d,%d" failed. Aborting tileset. Took %.1f sec (CPU %.1f). %d unknown tiles. Uploaded by %s (uuid %d)' % (tset.layer,tset.base_z,tset.x,tset.y,time()-starttime[0],clock()-starttime[1], unknown_tiles, self.upload.user_id,self.upload.client_uuid))
          else:
            # movetiles did not return a valid tileset
            logging.error('Unzipped file from user %s (uuid %d) was no valid tileset. Took %.1f sec (CPU %.1f).' % (self.upload.user_id,self.upload.client_uuid,time()-starttime[0],clock()-starttime[1]))
        self.cleanup(True)

      else:
        logging.info("uploaded file not found, deleting upload from user %s (uuid %d)." % (self.upload.user_id,self.upload.client_uuid))
	self.upload.delete()
   except KeyboardInterrupt:
     if self.upload: self.cleanup(False)
     logging.info('Ctrl-C pressed. Shutdown gracefully.')
     sys.exit("Ctrl-C pressed. Shutdown gracefully. Upload was: %s" % self.upload)
  #-----------------------------------------------------------------
  def unzip(self):
    now = clock()
    outfile = None
    self.tmptiledir = dir = os.path.join(self.unzip_path,self.uid)
    os.mkdir(dir, 0777)
    try:
      zfobj = zipfile.ZipFile(self.fname)

      for name in zfobj.namelist():
        if name.endswith('/'):
          os.mkdir(os.path.join(dir, name))
        else:
          outfile = open(os.path.join(dir, name), 'wb')
          outfile.write(zfobj.read(name))
          outfile.close()
    except zipfile.BadZipfile:
      logging.warning('found bad zip file %s uploaded by user %s', (self.uid, self.upload.user_id))
      if outfile: outfile.close()
      return 0
    except:
      logging.warning('unknown zip file error in file uploaded by user %s' % self.upload.user_id)
      if outfile: outfile.close()
      return(0)

    logging.debug('Unzipped tileset in %.1f sec.' % ((clock()-now)))
    return(1)

  #-----------------------------------------------------------------
  def movetiles(self):
    # 67 byte file => mark blank land;69 byte => mark blank sea; ignore every thing else below 100 bytes
    smalltiles = 0
    layer = None
    r = re.compile('^([a-zA-Z]+)_(\d+)_(\d+)_(\d+).png$')
    tset = Tileset()

    for f in os.listdir(self.tmptiledir):
      ignore_file = False
      full_filename = os.path.join(self.tmptiledir,f)
      m = r.match(f)
      if not m:
        logging.info('found weird file '+f+' in zip from user %s. Ignoring.' % self.upload.user_id)
        ignore_file = True

      if not ignore_file:
        # get the layer if it's any different
        if not (layer and layer.name == m.group(1)):
          try: layer = Layer.objects.get(name=m.group(1))
          except Layer.DoesNotExist:
            logging.info("unknown layer '%s' in upload by user %s" % (m.group(1), self.upload.user_id))
            return 0
        t = Tile(layer=layer,z=m.group(2),x=m.group(3),y=m.group(4))
        #print "found layer:"+m.group(1)+'z: '+m.group(2)+'x: '+m.group(3)+'y: '+m.group(4)

        #check the size to catch special meaning. This check can be removed once blankness
        #information is conveyed by other means than file size...
        fsize = os.stat(full_filename)[stat.ST_SIZE]
        if fsize < 100:
          if fsize == 67:
            #mark blank land
            t.set_blank_land()
            tset.add_tile(t,None)
          elif fsize == 69:
            #mark blank sea
            t.set_blank_sea()
            tset.add_tile(t,None)
          else:
            # ignore unknown small png files
            smalltiles += 1
        else:
          #png has regular filesize
          tset.add_tile(t,full_filename)

    if smalltiles: logging.debug('Ignored %d too small png files' % smalltiles)
    return tset

  #-----------------------------------------------------------------
  def add_user_stats(self, uploaded_tiles):
    """ Update the tah user statistics after a successfull upload """
    tahuser = self.upload.user_id.tahuser_set.get()
    try: tahuser.kb_upload += os.stat(self.upload.get_file_filename())[stat.ST_SIZE] // 1024
    except OSError: pass
    tahuser.renderedTiles += uploaded_tiles
    tahuser.save()

  #-----------------------------------------------------------------
  def sigterm(self, signum, frame):
    """ This is called when a SIGTERM signal is issued """
    print "Received SIGTERM signal. Shutdown gracefully."
    self.cleanup(None, False)
    sys.exit(0)

  #-----------------------------------------------------------------
  def cleanup(self, del_upload = True):
    """ Removes all temporary files and removes the upload object 
        (and the uploaded file if 'del_upload' is True.
    """
    # Delete the unzipped files directory
    shutil.rmtree(self.tmptiledir, True)
    self.uid=None
    self.fname=None
    self.tmptiledir=None
    if del_upload:
      # delete the uploaded file itself
      try: os.unlink(self.upload.get_file_filename())
      except: pass
      # delete the upload db entry
      self.upload.delete()

#---------------------------------------------------------------------

if __name__ == '__main__':
  config = Settings()
  logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(levelname)s %(message)s',
                    datefmt = "%y-%m-%d-%H:%M:%S", 
                    filename= config.getSetting(name='logFile'),
                    ) 

  logging.info('Starting tile upload processor')
  u = TileUpload(config)
  signal.signal(signal.SIGTERM,u.sigterm)
  if not u.process():
      logging.critical('Upload handling returned with error. Aborting.')
      sys.stderr.write('Upload handling returned with error')
      sys.exit(1)
else:
  sys.stderr.write('You need to run this as the main program.')
  sys.exit(1)
