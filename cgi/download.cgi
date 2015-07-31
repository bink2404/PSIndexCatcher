#!/usr/bin/perl

#======================================== --- -- -
#                                     
# Download Monitor
# By Gerrit Powire
# CGI Dynamics
# 
# Version: 0.99Beta
# Last Modified 11/1/2000
# Copyright (c) 2000
#
# Latest version is available from
# http://www.talkdungeon.com/dynamic
#
#============================= --- -- -
#
# COPYRIGHT NOTICE:
# 
# Copyright 2000 Gerrit W. Powrie All Rights Reserved.
# 
# This program is free to use for commercial and non commercial use. It
# is free because it is still in development and is a beta version. A 
# full shareware version will be available in future.
# 
# This script may be used and modified by anyone, so long as this copyright 
# notice and the header above remain intact.
# 
# Selling the code for this program without prior written consent 
# is expressly forbidden. You may redistribute this program as long
# as this copyright notice stay's intact and the code in it's original
# format. Redistribution should only take place with all the files in
# it's original .zip format where it was obtained. In all cases 
# copyright, header and original zip file must be original.
# 
# This program is distributed "as is" and without warranty of any
# kind, either express or implied. In no event shall the liability 
# of Gerrit W. Powrie and/or CGI Dynamics for any damages,
# losses and/or causes of action exceed the total amount paid by 
# the user for this software.
#
# iNSTaLLaTioN
# ============ === ==== ---- -- -
# See readme.txt
# HoPe You eNJoy THe SCRiPT
#
############################################################

############################################################
# Configuration - Edit below
############################################################

# Directory where data & download counts should be stored. You also 
# need to chmod this directory to 777. This directory location would 
# be best where users of your website cannot access it. Maybe 
# password protect the directory or use a directory that is out 
# of reach of the webserver.
$data_dir = "/home/htdocs/dlmonitor";

# URL Location of download files
$download_files = "http://www.yoursite.com/files";

# Server location of files to be downloaded
$download_loc = "/home/htdocs/dlmonitor/files";


###############################################################
# No more editing below - Unless you know what's happening here
###############################################################

if ($ENV{'QUERY_STRING'} ne "") {
								$temp = $ENV{'QUERY_STRING'};
								}
								else
								{
								read(STDIN, $temp, $ENV{'CONTENT_LENGTH'});
								}


@pairs=split(/&/,$temp);

foreach $item(@pairs) 	
	{
		($key,$content)=split (/=/,$item,2);
	    $content=~tr/+/ /;
		$content=~ s/%(..)/pack("c",hex($1))/ge;
		$fields{$key}=$content;
	}

	$fields{'comment'}=~s/\cM//g;
	$fields{'comment'}=~s/\n\n/<p>/g;
	$fields{'comment'}=~s/\n/<br>/g;

	$fields{'download'} =~ s/\;//g;
	$fields{'download'} =~ s/^\s+//g;
	$fields{'download'} =~ s/\s+$//g;

############################################################
############################################################
############################################################
############################################################

### ERROR CHECKING

$file_exists = (-e "$download_loc/$fields{'download'}");

if ($file_exists < 1)
	{
	&file_not_exist;
	exit;
	}
	


### OPERATIONS
#print "Content-type: text/html\n\n";

$exists1 = (-e "$data_dir/index.idx");
if ($exists1 > 0)
{
	open (DNL, "$data_dir/index.idx");
		while (defined($line=<DNL>))
			{
			####### filename, Number of Downloads
			($filename, $counter, $tmp) = split (/:-:/,$line, 3);
			
			#print "$filename __ $counter __ $tmp <br>";

				if ($filename eq "$fields{'download'}")
					{
					$counter++;
					$towrite = $towrite . $filename . ":-:" . $counter . ":-:" . "\n";
					$written = "true";
					}
					else
					{
					$towrite = $towrite . "$line"; #. "\n";
					}

			}
	close (DNL);
	
	if ($written ne "true")
		{
		open (DNL, ">> $data_dir/index.idx");
			print DNL $fields{'download'} . ":-:" . "1" . ":-:" . "\n";
		close (DNL);
		}
		else
		{
		open (DNL, "> $data_dir/index.idx");
			print DNL $towrite;
		close (DNL);		
		}
}
else
{
$counter = 1;

	if ($written ne "true")
		{
		open (DNL, "> $data_dir/index.idx");
			print DNL $fields{'download'} . ":-:" . "1" . ":-:" . "\n";
		close (DNL);
		}

}


##### RECORD INFO
$rfile = $fields{'download'};

$fields{'download'} =~ tr/./_/;  
$fields{'download'} = $fields{'download'} . ".txt";

$ip_address = $ENV{'REMOTE_ADDR'};
$browser = $ENV{'HTTP_USER_AGENT'};

($sec,$min,$hour,$mday,$mon,$year,$wday,$ydat,$isdst) = localtime();
$mon++;
$year = 1900 + $year;
$today = $year . "-" .  $mon . "-" . $mday . " " . $hour . ":" . $min . ":" . $sec;


open (DLD, ">> $data_dir/$fields{'download'}");
    ######### WEBURL					  HEAR OF				     NAME					   
	print DLD $fields{'weburl'} . ":-:" . $fields{'where'} . ":-:" . $fields{'name'} . ":-:" . 
	#EMAIL					   MAILING LIST            IP
	$fields{'email'} . ":-:" . $fields{'ml'} . ":-:" . $ip_address . ":-:" . $browser . ":-:" .
	#TODAY
	$today . ":-:\n";
close (DLD);

print "Location: $download_files/$rfile\n\n";
exit;






sub file_not_exist
{

print "Content-type: text/html\n\n";

print <<End_of_nexist;

<html>

<head>
<title>File Not Found</title>
</head>

<body>

<p>&nbsp;</p>

<p>&nbsp;</p>

<p>&nbsp;</p>
<div align="center"><center>

<table border="0" cellspacing="0" cellpadding="0" width="400">
  <tr>
    <td bgcolor="#000000"><table border="0" cellspacing="1" width="100%" cellpadding="6">
      <tr>
        <td width="100%" bgcolor="#FFCC00" valign="top"><p align="center"><font face="Verdana"
        size="2"><b>404 File Not Found</b></font></td>
      </tr>
      <tr>
        <td width="100%" bgcolor="#FFFFFF" valign="top"><font face="Verdana" size="2"><b></b>
        &nbsp;&nbsp;&nbsp; <br>
        The file you requested could not be found. Please use the back button of your browser to
        go back.<br>
        &nbsp;&nbsp;&nbsp; <b></b></font></td>
      </tr>
    </table>
    </td>
  </tr>
</table>
</center></div>
</body>
</html>

End_of_nexist

}



