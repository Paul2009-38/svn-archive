#!/usr/bin/ruby



require 'singleton'
load '/home/steve/bin/dao.rb'

puts '# Generated daily'

dao = OSM::Dao.instance

res = dao.call_sql { 'select format(avg(lat),6) as lat, format(avg(lon),6) as lon, part1, part2 from codes where confirmed = 1 group by part1, part2;' }


res.each_hash do |row|
  puts row['lat'] + ' ' + row['lon'] + ' ' + row['part1'] + ' ' + row['part2']
end

