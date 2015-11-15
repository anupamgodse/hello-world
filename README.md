Copyright (C) 2015  ANUPAM GODSE anupamgodse121@gmail.com

projectfdupes is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

projectfdupes is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

--------------------------------------------------------------------
Introduction
--------------------------------------------------------------------
projectfdupes is a program for identifying duplicate files residing
within specified directories. 


Usage
--------------------------------------------------------------------
Usage: 
		1. make
		Run the code as:
		2. >./project [options] DIRECTORY...(in ubuntu)

-f --omitfirst   omit the first file in each set of matches(while using 
		     this option with delete it will show all files).

-r --recurse     for every directory given follow subdirectories
                 encountered within.

-R --recurse:    for each directory given before this option does not
		     follow subdirectories encountered within but thosE
                 after this option follow subdirectories encountered within.

-1 --sameline    list each set of matches on a single line.

-S --size        show size for each set of matches.

-n --noempty     exclude zero-length files from consideration

-d --delete      prompt user for files to preserve and delete.

-N --noprompt    together with --delete, preserve the first file in
                 each set of duplicates and delete the rest without
                 prompting the user.

-A --nohidden    exclude hidden files from consideration.

-m --summarize   summarize duplicate files information.

-s --symlinks    follow symlinks.

-H --hardlinks   normally, when two or more files point to the same
		     disk area they are treated as non-duplicates; this 
                 option will change this behavior.

-h --help        display this help message.


When same directory is specified more than once each file will 
considered only once unless user tells explicitly to consider 
hardlinks(they are files with same path(same files)!!) because if
directories specified are same, of course each set of duplicate 
will list same files as many times as the same directory is spcified.
All files within that directory will be listed as their own
duplicates, leading to data loss should a user preserve a file 
without its "duplicate" (the file itself!).

When using -d or --delete, care should be taken to insure against
accidental data loss. While no information will be immediately
lost, using this option together with -s or --symlink can lead 
to confusing information being presented to the user when prompted
for files to preserve. Specifically, a user could accidentally
preserve a symlink while deleting the file it points to.

---------------------------------------------------------------------------------------------------------------------
