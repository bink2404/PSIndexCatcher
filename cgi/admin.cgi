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
# http://www.talkdungeon.com/dynamic/
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

########################################################################################
#Configuration of the script
########################################################################################

# Where is all comment data kept
$download_data = "/home/htdocs/dlmonitor";

# The url location of admin.cgi
$admin_cgi = "http://www.yoursite.com/cgi-bin/dlmonitor/admin.cgi";

#The username you plan to use when administirring the script
$username = "hello";

#The password you plan to use when administirring the script
$password = "password";


########################################################################################
# Don't change anything below this line unless you know what yer doing - :o)
########################################################################################

if ($ENV{'QUERY_STRING'} ne "") {
								$temp = $ENV{'QUERY_STRING'};
								}
								else
								{
								read(STDIN, $temp, $ENV{'CONTENT_LENGTH'});
								}

@pairs=split(/&/,$temp);

foreach $item(@pairs) {
	($key,$content)=split (/=/,$item,2);
	$content=~tr/+/ /;
	$content=~ s/%(..)/pack("c",hex($1))/ge;
	$fields{$key}=$content;
}

$fields{'comment'}=~s/\cM//g;
$fields{'comment'}=~s/\n\n/<p>/g;
$fields{'comment'}=~s/\n/<br>/g;


if (substr($fields{'pass'}, 0, 2) eq "_-"){$password = "_-" . crypt ($password, "comments");}


if ($fields{'usern'} eq "") {&passwordpage;}
if ($fields{'pass'} eq "") {&passwordpage;}

if ($fields{'usern'} ne $username) {&wrongpass;}
if ($fields{'pass'} ne $password) {&wrongpass;}


if ($fields{'usern'} eq $username) 
   {
	if ($fields{'pass'} eq $password) 
		
		{
			
		if ($fields{'fct'} eq "") {&start;}
		if ($fields{'fct'} eq "view_stats") {&view_stats;}
		if ($fields{'fct'} eq "generate") {&generate_ml;}
		if ($fields{'fct'} eq "reset_i") {&reset_i;}
		if ($fields{'fct'} eq "reset") {&reset_all;}
		if ($fields{'fct'} eq "generate_complete") {&generate_complete;}

		}

	}

exit;





########################################################################################################################

sub start
{

print "Content-type: text/html\n\n";

if ($fields{'login'} eq "login") { $password = "_-" . crypt ($password, "comments"); }

&print_top;

print <<End_of_fldo;

<div align="center"><center>
<table border="0" cellpadding="3" cellspacing="1" width="630">
  <tr>
    <td width="158" bgcolor="#FFCC00"><font face="Verdana" size="1" color="#000000"><strong>Filename</strong></font></td>
    <td width="157" bgcolor="#FFCC00"><font face="Verdana" size="1" color="#000000"><strong>Generate
    Mailing List</strong></font></td>
    <td width="158" bgcolor="#FFCC00"><font face="Verdana" size="1" color="#000000"><strong>Download
    Log Stats</strong></font></td>
    <td width="79" bgcolor="#FFCC00"><font face="Verdana" size="1" color="#000000"><strong>Downloads</strong></font></td>
    <td width="78" bgcolor="#FFCC00"><font face="Verdana" size="1" color="#000000"><strong>Reset
    to 0</strong></font></td>
  </tr>
</table>
</center></div>

End_of_fldo


open(DIR,"$download_data/index.idx");
	while (defined($line=<DIR>))
	{
		($filename, $no_downloads, $tmp)=split (/:-:/,$line,3);

$dfile = $filename;
$dfile =~ tr/./_/;

print <<End_of_s;

<div align="center"><center>

<table border="0" cellpadding="3" cellspacing="1" width="630">
  <tr>
    <td width="158" bgcolor="#FFF8D7"><font face="Verdana" size="1" color="#000000">$filename</font></td>
    <td width="157" bgcolor="#FFF8D7"><font face="Verdana" size="1" color="#000000"><a
    href="$admin_cgi?fct=generate&amp;usern=$username&amp;pass=$password&amp;fl=$dfile">Generate</a></font></td>
    <td width="158" bgcolor="#FFF8D7"><font face="Verdana" size="1" color="#000000"><a
    href="$admin_cgi?fct=view_stats&amp;usern=$username&amp;pass=$password&amp;fl=$dfile">View
    Download Log</a></font></td>
    <td width="79" bgcolor="#FFF8D7"><font face="Verdana" size="1" color="#000000">$no_downloads</font></td>
    <td width="78" bgcolor="#FFF8D7"><font face="Verdana" size="1" color="#000000"><a
    href="$admin_cgi?fct=reset_i&amp;usern=$username&amp;pass=$password&amp;fl=$dfile&fln=$filename">Reset</a></font></td>
  </tr>
</table>
</center></div>

End_of_s

$stat1found = "true";
		
	}
close(DIR);


if ($stat1found eq "")
	{

print <<End_of_nd;

<div align="center"><center>
<table border="0" cellpadding="3" cellspacing="1" width="630">
  <tr>
    <td width="630" bgcolor="#FFF8D7"><font face="Verdana" size="1" color="#000000">No
    downloads has taken place yet.</font></td>
  </tr>
</table>
</center></div>

End_of_nd
	
	}


print <<End_of_start;

<div align="center"><center>

<table border="0" cellspacing="1" width="630" cellpadding="3">
  <tr>
    <td width="100%" bgcolor="#FFCC00"><font color="#000000" face="Verdana" size="1"><strong>Click
    on an option above.</strong></font></td>
  </tr>
  <tr>
    <td width="100%"><font color="#000000" face="Verdana" size="1"><strong>&nbsp;&nbsp;&nbsp;&nbsp;
    </strong></font></td>
  </tr>
  <tr>
    <td width="100%" bgcolor="#FFCC00"><font color="#000000" face="Verdana" size="1"><strong>Admin
    Functions</strong></font></td>
  </tr>
  <tr>
    <td width="100%" bgcolor="#FFFBEA"><form method="POST" action="$admin_cgi">
      <input type="hidden" name="fct" value="reset"><input type="hidden" name="pass"
      value="$password"><input type="hidden" name="usern" value="$username"><p><font
      face="Verdana" size="1">&nbsp;&nbsp;&nbsp; <br>
      Reset All Statistics and Downloads <input type="checkbox" name="reset" value="Y"><input
      type="submit" value="    Reset   " name="B1"
      style="font-family: sans-serif; font-size: 8pt"></font></p>
    </form>
    </td>
  </tr>
  <tr>
    <td width="100%" bgcolor="#FFFBEA"><form method="POST" action="$admin_cgi">
      <input type="hidden" name="fct" value="generate_complete"><input type="hidden" name="pass"
      value="$password"><input type="hidden" name="usern" value="$username"><p><font
      face="Verdana" size="1">Generate a mailing list for all email address in logs: <input
      type="submit" value="    Generate   " name="B1"
      style="font-family: sans-serif; font-size: 8pt"></font></p>
    </form>
    </td>
  </tr>
</table>
</center></div>

<p align="center"><font face="Verdana" size="2">Copyright ©2000, <a
href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></font></p>
</body>
</html>

End_of_start
	
exit;
}


###########################################################################


sub passwordpage

{
	
print "Content-type: text/html\n\n";
print <<End_of_pp;

<html>

<head>
<title>Download Monitor</title>
</head>

<body bgcolor="#FFFFFF" text="#000000">
<div align="center"><center>

<table border="0" cellspacing="0" cellpadding="0" width="630">
  <tr>
    <td bgcolor="#000000"><table border="0" cellspacing="1" width="100%" cellpadding="2">
      <tr>
        <td width="100%" bgcolor="#8080FF" valign="top"><p align="center"><font face="Verdana"
        size="2" color="#FFFFFF"><b>Download Monitor</b></font></td>
      </tr>
    </table>
    </td>
  </tr>
</table>
</center></div>

<hr width="630" size="1">
<div align="center"><center>

<table border="0" cellpadding="0" cellspacing="0" width="630" bgcolor="#000000">
  <tr>
    <td width="100%" bgcolor="#000000"><table border="0" cellspacing="1" width="100%"
    bgcolor="#000000">
      <tr>
        <td width="100%" bgcolor="#FFCC00"><p align="center"><font color="#000000"><strong><font
        face="Arial"><big><big>Welcome</big></big></font></strong><br>
        <font face="Verdana"><small>Please Insert Your User Name and Password</small></font></font></p>
        <form method="POST" action="admin.cgi" $adminpath; vadmin.cgi>
          <input type="hidden" name="login" value="login"><div align="center"><center><p><font
          face="Verdana"><font color="#000000"><strong><small>User Name :</small></strong><br>
          </font><input type="text" name="usern" size="13"><br>
          <font color="#000000"><strong><small>Password :</small></strong><br>
          </font><input type="password" name="pass" size="13"><br>
          <br>
          <strong><small><input type="submit" value="LogIn" name="B1"></small></strong></font></p>
          </center></div>
        </form>
        <p align="center"><font face="Verdana">&nbsp;</font></p>
        <p align="center"><font face="Verdana"><small>Copyright ©2000, <a
        href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></small><br>
        <small>&nbsp;&nbsp; </small></font></td>
      </tr>
    </table>
    </td>
  </tr>
</table>
</center></div>

<p>&nbsp;</p>
</body>
</html>
	
End_of_pp
	
	exit;
	
}




###########################################################################

sub wrongpass {
	
print "Content-type: text/html\n\n";

print <<End_of_wrong;

<html>

<head>
<title>Download Monitor</title>
</head>

<body bgcolor="#FFFFFF" text="#000000">
<div align="center"><center>

<table border="0" cellspacing="0" cellpadding="0" width="630">
  <tr>
    <td bgcolor="#000000"><table border="0" cellspacing="1" width="100%" cellpadding="2">
      <tr>
        <td width="100%" bgcolor="#8080FF" valign="top"><p align="center"><font face="Verdana"
        size="2" color="#FFFFFF"><b>Download Monitor</b></font></td>
      </tr>
    </table>
    </td>
  </tr>
</table>
</center></div>

<hr width="630" size="1">
<div align="center"><center>

<table border="0" cellpadding="0" cellspacing="0" width="630" bgcolor="#000000">
  <tr>
    <td width="100%" bgcolor="#000000"><table border="0" cellspacing="1" width="100%"
    bgcolor="#000000">
      <tr>
        <td width="100%" bgcolor="#FFCC00"><p align="center"><font color="#000000"><strong><font
        face="Arial"><big><big>Welcome</big></big></font></strong><br>
        <font face="Verdana"><small>Please Insert Your User Name and Password</small></font></font></p>
        <p align="center"><font color="#000000" face="Verdana"><strong><small>Sorry, the password
        your entered was incorrect</small></strong></font></p>
        <form method="POST" action="admin.cgi" $adminpath; vadmin.cgi>
          <input type="hidden" name="login" value="login"><div align="center"><center><p><font
          face="Verdana"><font color="#000000"><strong><small>User Name :</small></strong><br>
          </font><input type="text" name="usern" size="13"><br>
          <font color="#000000"><strong><small>Password :</small></strong><br>
          </font></font><font face="Arial"><input type="password" name="pass" size="13"><br>
          <br>
          <strong><small><input type="submit" value="LogIn" name="B1"></small></strong></font></p>
          </center></div>
        </form>
        <p align="center">&nbsp;</p>
        <p align="center"><font face="Arial"><small>Copyright ©2000, <a
        href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></small><br>
        <small>&nbsp;&nbsp; </small></font></td>
      </tr>
    </table>
    </td>
  </tr>
</table>
</center></div>

<p>&nbsp;</p>
</body>
</html>

End_of_wrong

exit;
	
}






################################################################################################

sub print_top
{

print <<End_of_top;

<html>

<head>
<title>Quick Page Commenter Admin Area</title>
</head>

<body>
<div align="center"><center>

<table border="0" cellspacing="0" width="630" bgcolor="#000000" cellpadding="0">
  <tr>
    <td width="100%"><div align="center"><center><table border="0" cellspacing="1"
    width="100%" cellpadding="4">
      <tr>
        <td width="100%" bgcolor="#8080FF"><font color="#FFFFFF" face="Verdana"><strong><small>Download
        Monitor Admin / Stats Area</small></strong></font></td>
      </tr>
    </table>
    </center></div></td>
  </tr>
</table>
</center></div>

<hr width="630" size="1">


End_of_top


}





sub view_stats
{

print "Content-type: text/html\n\n";

&print_top;

print <<End_of_topl;

<p><font face="MS Sans Serif" size="1"><strong>DOWNLOAD LOG</strong><br>
Date | Web url | Where did downloader hear of this program | Name | Email | Does he want
to receive email of updates and new software | Ip Address | Browser</font></p>

<hr size="1">

End_of_topl


open (DLF, "$download_data/$fields{'fl'}.txt");
	
	print "<font face=\"MS Sans Serif\" size=\"1\">";
	
	while (defined($line=<DLF>))
	{
	($weburl, $refer, $name, $email, $mailinglist, $ip, $browser, $date)=split (/:-:/,$line,9);
	print "<b>$date</b> | $weburl | $refer | $name | $email | $mailinglist | $ip | $browser <br><br>";
	}	

	print "</font><hr size=\"1\">";
	
close (DLF);


print <<End_of_page;

<p align="center"><a href="$admin_cgi?usern=$username&amp;pass=$password"><strong><font
size="2" face="Verdana">Download Monitor Admin Home</font></strong></a></p>

<p align="center"><font face="Verdana" size="2">Copyright ©2000, <a
href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></font></p>
</body>
</html>

End_of_page

}






sub generate_ml
{


print "Content-type: text/html\n\n";

&print_top;

$filename = $fields{'fl'};
$filename =~ tr/_/./;

print <<End_of_df;

<blockquote>
  <blockquote>
    <blockquote>
      <p><strong><small><font face="Verdana">Email Addresses of users who downloaded $filename</font></small></strong></p>
    </blockquote>
  </blockquote>
</blockquote>

End_of_df


print "<blockquote><blockquote><blockquote>";

open (DLF, "$download_data/$fields{'fl'}.txt");

	while (defined($line=<DLF>))
	{
	($weburl, $refer, $name, $email, $mailinglist, $ip, $browser, $date)=split (/:-:/,$line,9);
		if (($email ne "") and ($mailinglist = "Y"))
			{
			print "$email<br>";
			$efound = "true";
			}
	}	
	
close (DLF);

print "</blockquote></blockquote></blockquote>";


if ($efound ne "true")
	{

print <<End_of_cp;

<blockquote>
  <blockquote>
    <blockquote>
      <p><small><strong><font face="Verdana">No email addresses was captured yet.</font></strong></small></p>
    </blockquote>
  </blockquote>
</blockquote>

End_of_cp
	
	}


print <<End_of_page;

<div align="center"><center>

<table border="0" cellspacing="0" width="630" bgcolor="#000000" cellpadding="0">
  <tr>
    <td width="100%"><div align="center"><center><table border="0" cellspacing="1"
    width="100%" cellpadding="4">
      <tr>
        <td width="100%" bgcolor="#FFCC00"><strong><font size="2" face="Verdana">Copy and paste
        the email addresses into your mailing list software.</font></strong></td>
      </tr>
    </table>
    </center></div></td>
  </tr>
</table>
</center></div>

<hr width="630" size="1">

<p align="center"><a href="$admin_cgi?usern=$username&amp;pass=$password"><strong><font
size="2" face="Verdana">Download Monitor Admin Home</font></strong></a></p>

<p align="center"><font size="2" face="Verdana">Copyright ©2000, <a
href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></font></p>
</body>
</html>

End_of_page

}





sub reset_i
{

print "Content-type: text/html\n\n";
&print_top;



	open (DNL, "$download_data/index.idx");
		while (defined($line=<DNL>))
			{
			####### filename, Number of Downloads
			($filename, $counter, $tmp) = split (/:-:/,$line, 3);

				if ($filename eq "$fields{'fln'}")
					{
					$towrite = $towrite . $filename . ":-:" . "0" . ":-:" . "\n";
					$written = "true";
					}
					else
					{
					$towrite = $towrite . "$line";
					}
			}
	close (DNL);

	open (DNL, "> $download_data/index.idx");
		print DNL $towrite;
	close (DNL);		

	unlink "$download_data/$fields{'fl'}.txt";


print <<End_of_page;

<div align="center"><center>

<table border="0" cellspacing="0" width="630" bgcolor="#000000" cellpadding="0">
  <tr>
    <td width="100%"><div align="center"><center><table border="0" cellspacing="1"
    width="100%" cellpadding="4">
      <tr>
        <td width="100%" bgcolor="#FFCC00"><font size="2" face="Verdana"><strong>Statistics for
        $filename have been reset</strong></font></td>
      </tr>
    </table>
    </center></div></td>
  </tr>
</table>
</center></div>

<p align="center"><a href="$admin_cgi?usern=$username&amp;pass=$password"><strong><font
size="2" face="Verdana">Download Monitor Admin Home</font></strong></a></p>

<p align="center"><font face="Verdana" size="2">Copyright ©2000, <a
href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></font></p>
</body>
</html>

End_of_page

}




sub reset_all
{

print "Content-type: text/html\n\n";
&print_top;

if ($fields{'reset'} eq "Y")
	{

		open(DIR,"$download_data/index.idx");
			while (defined($line=<DIR>))
				{
				($filename, $counter, $tmp) = split (/:-:/,$line, 3);
				$filename =~ tr/./_/;
				$filename = $filename . ".txt";
				#print "==> $filename";
				unlink ("$download_data/$filename");
				}
		close (DIR);

unlink ("$download_data/index.idx");

print <<End_of_rs;

<div align="center"><center>
<table border="0" cellspacing="1" width="630" cellpadding="3">
  <tr>
    <td width="100%"><font color="#000000" face="Verdana" size="1">
	<strong>All Log Files and Counts has been reset</strong></font></td>
  </tr>
</table>
</center></div>

<p align="center"><a href="$admin_cgi?usern=$username&amp;pass=$password"><strong><font
size="2" face="Verdana">Download Monitor Admin Home</font></strong></a></p>
</body>
</html>

End_of_rs
	

	}
	else
	{

print <<End_of_rs;

<div align="center"><center>

<table border="0" cellspacing="1" width="630" cellpadding="3">
  <tr>
    <td width="100%"><font color="#000000" face="Verdana" size="1"><strong>You have to check
    the checkbox in order to reset all stats.</strong></font></td>
  </tr>
</table>
</center></div>

<p align="center"><a href="$admin_cgi?usern=$username&amp;pass=$password"><strong><font
size="2" face="Verdana">Download Monitor Admin Home</font></strong></a></p>
</body>
</html>

End_of_rs
	
	}



}




sub generate_complete
{

print "Content-type: text/html\n\n";
&print_top;

$emno = 0;

print "<blockquote><blockquote><blockquote>";

	open(DIR,"$download_data/index.idx");
		while (defined($line=<DIR>))
			{
			($filename, $counter, $tmp) = split (/:-:/,$line, 3);
			
			$filename =~ tr/./_/;
			$filename = $filename . ".txt";
			
				open (EML, "$download_data/$filename");
					while (defined($line2=<EML>))
						{
						($weburl, $refer, $name, $email, $mailinglist, $ip, $browser, $date)=split (/:-:/,$line2,9);
						   if (($email ne "") and ($mailinglist = "Y"))
						   	{
							print "$email <br>";
							$emno++;
							}
						}
				close (EML);
				
			}
	close (DIR);

print "</blockquote></blockquote></blockquote>";


if ($emno == 0)
	{
	print "<blockquote><blockquote><blockquote><font size=\"2\" face=\"Verdana\"><strong>No Email Addresses Recorded Yet</strong></font></blockquote></blockquote></blockquote>";
	}


print <<end_of_end;
<br>
<div align="center"><center>

<table border="0" cellspacing="0" width="630" bgcolor="#000000" cellpadding="0">
  <tr>
    <td width="100%"><div align="center"><center><table border="0" cellspacing="1"
    width="100%" cellpadding="4">
      <tr>
        <td width="100%" bgcolor="#FFCC00"><font size="2" face="Verdana"><strong>$emno Email
        Addresses</strong></font></td>
      </tr>
    </table>
    </center></div></td>
  </tr>
</table>
</center></div>

<hr width="630" size="1">

<p align="center"><a href="$admin_cgi?usern=$username&amp;pass=$password"><strong><font
size="2" face="Verdana">Download Monitor Admin Home</font></strong></a></p>
<p align="center"><font size="2" face="Verdana">Copyright ©2000, <a
href="http://www.talkdungeon.com/dynamic/">CGI Dynamics</a></font></p>
</body>
</html>

end_of_end



}


