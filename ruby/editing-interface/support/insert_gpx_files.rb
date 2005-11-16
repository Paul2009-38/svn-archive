#!/usr/bin/ruby

# FIXME:
# reject points which go too quickly

require 'osm/dao'
require 'time'
require 'zlib'
require 'rexml/parsers/sax2parser'
require 'rexml/text'
require 'net/smtp'

DEBUG = false

# exit if the lockfile is there
if File.file?('/tmp/gpx_insert_running')
  $stderr << 'exiting, lock file present'
  exit!
else
  #otherwise make one
  File.open('/tmp/gpx_insert_running', 'w') do |file|
    file << 'eh-oh'
  end
end

dao = OSM::Dao.instance

dao.delete_sheduled_gpx_files unless DEBUG

files = dao.get_scheduled_gpx_uploads

files.each_hash do |row|
  filename = row['tmpname']
  #copy the file here
  `scp 128.40.59.140:#{filename} .#{filename}`

  `zcat /home/steve/bin#{filename} > /home/steve/bin#{filename}.yeah`

  realfile = ''
  $stderr << 'file exists: ' + File.file?('.' + filename + '.yeah').to_s

  if File.file?('.' + filename + '.yeah') && File.size('.' + filename + '.yeah') > 0
    realfile = '.' + filename + '.yeah'
  else
    realfile = '.' + filename
  end

  puts realfile

  points = 0
  poss_points = 0
  user_uid = row['user_uid'].to_i
  original_name = row['originalname']
  email_address = dao.email_from_user_uid(user_uid)
  dbh = dao.get_connection #bit hacky, but we need a connection

  if File.file?( realfile ) && File.size( realfile ) > 0
    file = File.new( realfile )
    parser = REXML::Parsers::SAX2Parser.new( file )
  
    # got a file, we hope

    $stderr << 'Inserting ' + original_name + ' for user ' + user_uid.to_s + ' from file ' + filename + "\n"
  
    trackseg = 0
    if DEBUG
      gpx_uid = -1
    else
      gpx_uid = dao.new_gpx_file(user_uid, original_name) 
    end

    if gpx_uid == 0
      $stderr << "bad gpx number!\n"
      exit
    end

    $stderr << 'new gpx file uid: ' + gpx_uid.to_s + "\n"

    lat = -1
    lon = -1
    ele = -1
    date = -1
    gotlatlon = false
    gotele = false
    gotdate = false

    parser.listen( :start_element,  %w{ trkpt }) do |uri,localname,qname,attributes| 
      lat = attributes['lat'].to_f
      lon = attributes['lon'].to_f
      gotlatlon = true
      poss_points += 1
    end
  
    parser.listen( :characters, %w{ ele } ) do |text|
      ele = text
      gotele = true
    end

    parser.listen( :characters, %w{ time } ) do |text|
      if text && text != ''
        date = Time.parse(text).to_i * 1000
        gotdate = true
      end
    end

    parser.listen( :end_element, %w{ trkseg } ) do |uri, localname, qname|
      trackseg += 1
    end
  
    parser.listen( :end_element, %w{ trkpt } ) do |uri,localname,qname| 
      if gotlatlon && gotdate
        ele = '0' unless gotele
        if lat < 90 && lat > -90 && lon > -180 && lon < 180
          sql = "insert into tempPoints (latitude, longitude, altitude, timestamp, uid, hor_dilution, vert_dilution, trackid, quality, satellites, last_time, visible, dropped_by, gpx_id) values (#{lat}, #{lon}, #{ele}, #{date}, #{user_uid}, -1, -1, #{trackseg}, 255, 0, #{Time.new.to_i * 1000}, 1, 0, #{gpx_uid})"
          points += 1
          dbh.query(sql) unless DEBUG
        end
 
      end
      gotlatlon = false
      gotele = false
      gotdate = false

      #puts lat.to_s + ' ' + lon.to_s + ' ' + ele.to_s + ' ' + date.to_s
    end

    error = false;
    error_msg = ''
    begin
      parser.parse
      dao.update_gpx_meta(gpx_uid) unless DEBUG

    rescue Exception => e
      error = true
      error_msg = e.to_s
   
    end

  
    #get rid of the file so we don't insert it again

    #send them an email indicating success

  if email_address && email_address != ''
    msgstr = ''
    if !error
      msgstr = <<END_OF_MESSAGE
From: webmaster <webmaster@openstreetmap.org>
To: #{email_address}
Bcc: steve@fractalus.com
Subject: Your gpx file

Hi,

It looks like your gpx file, 
  
  #{original_name},

uploaded to OpenStreetMap's database ok with #{points} out of
a possible #{poss_points} track points in the GPX.

Have fun


END_OF_MESSAGE
    
    else
      error = false
      msgstr = <<END_OF_MESSAGE
From: webmaster <webmaster@openstreetmap.org>
To: #{email_address}
Bcc: steve@fractalus.com
Subject: Your gpx file

Hi,

It looks like your gpx file, #{original_name}, failed to get parsed ok.
Please consult the error message below. If you think this is a bug please
have a look at how to report it:

  http://www.openstreetmap.org/wiki/index.php/Bug_Reporting

#{error_msg}

END_OF_MESSAGE


    end
      if !DEBUG
        Net::SMTP.start('127.0.0.1', 25) do |smtp|
          smtp.send_message msgstr.untaint,
                            'webmaster@openstreetmap.org'.untaint,
                            email_address.untaint

        end
      else
        puts 'I would send this email:'
        puts msgstr
      end
       
   end

  else
    # nil file encountered
    errortext = ''
      if original_name == ''
        errortext = '(blank file name)'
      else
        errortext = original_name
      end
      
      msgstr = <<END_OF_MESSAGE
From: webmaster <webmaster@openstreetmap.org>
To: #{email_address}
Bcc: steve@fractalus.com
Subject: Your gpx file

Hi,

Your gpx file, '#{errortext}' didn't get uploaded because it was
empty. This usually means it didn't upload from your browser correctly.
This can happen, try it again. If errors persist, please report a bug:

  http://www.openstreetmap.org/wiki/index.php/Bug_Reporting

have fun


END_OF_MESSAGE
    if !DEBUG
      Net::SMTP.start('127.0.0.1', 25) do |smtp|
        smtp.send_message msgstr.untaint,
                         'webmaster@openstreetmap.org'.untaint,
                          email_address.untaint
      end
    else
      puts 'I would send this email:'
      puts msgstr
    end

  end

  if !DEBUG
    dbh.query("delete from gpx_to_insert where tmpname = '#{filename}'")
    File.delete('/home/steve/bin' + filename)
    File.delete('/home/steve/bin' + filename + '.yeah')
   `ssh 128.40.59.140 rm #{filename}`
  end

end

File.delete('/tmp/gpx_insert_running')

