#!/usr/bin/perl

my $buffer; 
my $name ;
my $value;
my $pair;
my @pairs ;
my $FORM ;
my $DATA ;
my $DATA2 ;
my $FILEDATA ;
my $usr_name ;
my $email ;
my $comments ;

# Read the input fields
read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});

# Split the name-value pairs perl file io
@pairs = split(/&/, $buffer); 

foreach $pair (@pairs) 
{ 
	($name, $value) = split(/=/, $pair); 

	$value =~ tr/+/ /; 
	$value =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg; 
	$name =~ tr/+/ /; 
	$name =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg; 

	$FORM{$name} = $value; 
} 

# Print the http header
print "Content-type: text/html\n\n";

print "<html><head><title>CGI test</title></head>\n";
print "<body>\n";

# Write the data to a file
open(DATA,">>data.txt") or dienice("Couldn't open data.txt writing: $!");

flock(DATA,2);
# Reset the file pointer to the end of the file, in case 
# someone wrote to it while we waited for the lock...
seek(DATA,0,2);

print DATA "$FORM{'usrname'}:$FORM{'email'}:$FORM{'comments'}\n";
close(DATA);

# Now read through the file and print out the contents
open(DATA2,"data.txt") or dienice("Couldn't open data.txt writing: $!");

flock(DATA2,2);

@raw_data=<DATA2>;

foreach $pair (@raw_data) 
{
	chop($pair);
    ($usr_name,$email,$comments)=split(/:/,$pair);

	print "USRNAME = $usr_name\n";
	print "EMAIL = $email\n";
	print "COMMENTS = $comments\n";

	print "<BR>";
}

close(DATA2);

print "</body></html>\n";
