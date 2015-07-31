+============================ === ==== ---- -- -
| Download Monitor V0.99Beta
+=========== === ==== ---- -- -

|This text file is best viewed in notepad
+=========== === ==== ---- -- -

COPYRIGHT NOTICE:

Copyright 2000 Gerrit W. Powrie All Rights Reserved.

This program is free to use for commercial and non commercial use. It
is free because it is still in development and is a beta version. A 
full shareware version will be available in future.

This script may be used and modified by anyone, so long as this copyright 
notice and the header above remain intact.

Selling the code for this program without prior written consent 
is expressly forbidden. You may redistribute this program as long
as this copyright notice stay's intact and the code in it's original
format. Redistribution should only take place with all the files in
it's original .zip format where it was obtained. In all cases 
copyright, header and original zip file must be original.

This program is distributed "as is" and without warranty of any
kind, either express or implied. In no event shall the liability 
of Gerrit W. Powrie and/or CGI Dynamics for any damages,
losses and/or causes of action exceed the total amount paid by 
the user for this software.


=================== === ==== ---- -- -
| WHaT THe SCRiPT eXaCTLy DoeS
============ === ==== ---- -- -
This script allows you to track file downloads from your web site. It will
also allow you to build a mailing list so that you can email the users
who downloaded your software/downloads to be informed of updates/changes or
new releases of the downloads. 

It comes with a highly functional admin interface where you can see how
many downloads of a particular file was made. You can also track
an unlimited amount of files with detailed count/download statistics 
for each file.

You will need mailing list software. Download monitor will only record 
the email addresses. Download Monitor will provide all email addresses
in a formatted list which you can copy and paste into your mailing
list software.


============ === ==== ---- -- -
| ReQuiReMeNTS
======= === == --- -- - 
You will need a unix or windows platform and CGI Priviledges on your webserver


============ === ==== ---- -- -
| iNSTaLLaTioN
============ === ==== ---- -- -

1. Unzip the zip file in which the files came. Open download.cgi with your 
favourite text editor. Make sure that the first line in the script points 
to you Perl intepreter on your server.

Example: #!/usr/bin/perl

2. Change the variables in download.cgi to point to the correct locations of 
your server. (You'll find these where the words 'Configuration - Edit below' 
is written in the script.)

    [Here are the variables that should be changed:]
	
	$data_dir		->This is the directory where recorded counts and log data
					  will be stored. 
					  You also need to chmod this directory to 777. This directory
					  location would be best where users of your website cannot
					  access it. Maybe password protect the directory or use a directory
					  that is out of reach of the webserver.

	$download_files	->The URL location of files to be downloaded. Download monitor will
					  retrieve the files here to pass it on to be downloaded by users.

	$download_loc	->The directory path where all files to be downloaded are situated. 
					  Download monitor will retrieve the files here to pass it on to 
					  be downloaded by users. Example: "/home/htdocs/downloadfiles"
	
After you completed this operation, remember to save download.cgi as a unix file if 
you're planning on using it on a unix machine. If you're planning to use it on a 
windows machine, remember to save it as a windows file.

download.cgi should now be configured!


3. Open admin.cgi and modify the following variables

	$data_dir		->This is the directory where recorded counts and log data
					  will be stored. 
					  You also need to chmod this directory to 777. This directory
					  location would be best where users of your website cannot
					  access it. Maybe password protect the directory or use a directory
					  that is out of reach of the webserver.
	
	$admin_cgi      ->The url location of admin.cgi
	
	$username		->The user name to log into the admin area.
	
	$password		->The password to log into the admin area.


4. Upload download.cgi and admin.cgi to your unix/windows machine. chmod 755 the scripts.

5. Create a directory called 'dlmonitor' or whatever you made the $data_dir variable in 
download.cgi - chmod 777 this directory to 777

This directory location would be best where users of your website cannot access it. 
Maybe password protect the directory or use a directory that is out of reach of the 
webserver.

6. Ok, by now the scripts should be ready for action, but you will have to insert the
download options (HTML CODE) into your web pages where you would want files to be 
downloaded. You will also have to place the files which are to be downloaded in 
the download location which you specified for $download_loc in download.cgi

Have a look at download.html that came with this installation package.

When opening download.html and looking at example 1, you will see that there are
many boxes that the user can fill in before downloading files. All these boxes
are optional, but the drop down menu with the name 'download' should contain 
the names of files/programs and their values should be the filename.

All the boxes are optional except the 'download' drop down menu.


ADMIN.CGI
=========
When you execute admin.cgi you will be prompted for a user name and password. The
default user name is: hello
and the password is: welcome
or whatever you specified when configuring admin.cgi

Admin.cgi will display detailed stats of all downloads.


iNSTaLLaTioN TRouBLe SHooTiNG
============ === ==== ---- -- -
1. Did you chmod download.cgi and admin.cgi to 755

2. Is download.cgi and admin.cgi saved in unix file format - If ran on a unix machine?

3. Is the $data_dir directory chmod'ed to 777. 
   Example: chmod 777 /home/httpd/html/someplace/dlmonitor



HoPe You eNJoy THe SCRiPT
============ === ==== ---- -- -
Gerrit Powrie

Please send bug reports to: gerrit@talkdungeon.com
