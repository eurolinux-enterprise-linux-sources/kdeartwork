#!/usr/bin/env ruby
#
# Copyright (C) 2008 Harald Sitter <apachelogger@ubuntu.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License or (at your option) version 3 or any later version
# accepted by the membership of KDE e.V. (or its successor approved
# by the membership of KDE e.V.), which shall act as a proxy
# defined in Section 14 of version 3 of the license.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

require 'fileutils'
@fu = FileUtils

texts  = [ "css", "sgml", "x-generic", "enriched", "rtf" ]
videos = [ "mp4", "mpeg", "x-flic", "x-generic", "x-matroska",
           "x-ms-asf", "x-msvideo", "x-ms-wmv", "x-ogm+ogg" ]
images = [ "bmp", "cgm", "fax-g3", "gif", "jp2", "jpeg", "png",
           "tiff", "x-applix-graphics", "x-dcraw", "x-dds", "x-exr", "x-fits",
           "x-generic", "x-hdr", "x-ico", "x-jng", "x-msod", "x-pcx",
           "x-photo-cd", "x-portable-bitmap", "x-portable-graymap",
           "x-portable-pixmap", "x-rgb", "x-tga", "x-xbitmap", "x-xcf",
           "x-xpixmap" ]
audios = [ "aac", "ac3", "basic", "mp4", "mpeg", "x-adpcm", "x-aiff",
           "x-flac+ogg", "x-flac", "x-generic", "x-matroska", "x-mod",
           "x-monkey", "x-mp2", "x-mpegurl", "x-ms-wma", "x-musepack",
           "x-scpls", "x-speex+ogg", "x-vorbis+ogg", "x-wav" ]

# TODO
# hash   = {texts,videos,images,audios}

def process(type, name)
    unless File.size?("#{type}-template.svgz") == File.size?("#{type}-#{name}.svgz")
        system("svn rm #{type}-#{name}.svgz")
        system("svn cp #{type}-template.svgz #{type}-#{name}.svgz")
        @file << "#{type}-template.svgz COPIED TO #{type}-#{name}.svgz\n"
# NOTE: debugging stuff
#         @fu.rm_f("#{type}-#{name}.svgz")
#         system("svn revert #{type}-#{name}.svgz")
    else
        puts("#{type}-template.svgz and #{type}-#{name}.svgz are identical \\o/")
    end
end

if File.exist?("./commit")
    puts("Commit file already exists, do you want to replace it? [Y/n]")
    s = gets.chomp()
    if s == "\n"
        s = "yes"
    end
end
@file = File.new("./commit", File::RDWR)
str = @file.read()
@file.rewind()
@file.truncate( 0 )
if (not s.downcase == "y" and not s.downcase.include?("yes")) \
and (s.downcase == "n" or s.downcase.include?("no"))
    @file << str
end

Dir.chdir("scalable")
Dir.chdir("mimetypes")

# TODO
# for part in hash
#     for name in part
#         process(part,name)
#     end
# end

for name in texts
    process("text",name)
end

for name in videos
    process("video",name)
end

for name in images
    process("image",name)
end

for name in audios
    process("audio",name)
end

@file.close()
