#
# Updates the NS manual using Balance.txt
#
local ($kVersion, $kFileSpecifier, $kBalanceFileName, $kTempFileName,%BalanceVars,@BalanceUpdateTime);

#
# Changelog
#
# v1.1 - Karl Patrick
#
# tag parser / content replacer
# - changed tag structure to be SGML compliant and use an ns namespace for ease of validation, e.g. <ns:value name="kParameterName">value</ns:value>
# - widened definition of content between nsvalue tags to include all characters except '<' so non-word characters can become placeholders
# - added global processing switch for the tag substitution operators (they now replace multiple matches in a line)
# - changed nsinverse tag to print inverse value with 2 decimal places
# - changed nspercent tag to print percentile after the numeric value and never use a decimal place
# - added fourth tag type, nsdate, which inserts the timestamp of the Balance.txt file in whatever format the tag specifies
# - TODO: use m// operator to match ns namespace tags and formally parse tag attributes
# - TODO: parse entire file at once to lose dependancy on line breaks
# file handling
# - added optional command line statements to manually specify the balance file name [arg0] and files to parse [all other args]
#	- default behavior remains unchanged if no args are supplied
#	- the current directory is scanned for files to update if only one argument is given
# - moved filtering of file names to default array construction; any explicitly named file will now be processed
# - replaced native system calls for file handling with perl function calls
# - added early termination of program on balance.txt file read failure
# - added automatic deletion of temp file if no replacements were made to the original
# - TODO: drop use of temp file in favor of local string storage, use rename-to-.bak and update paradigm for files
# misc
# - removed previously commented out debug print statements
# - replaced some of the globals with subroutine arguments / locals
# - changed output format to be easier to scan for large numbers of files
#
# v1.0 Original Version - Charlie Cleveland
#

#
# Constants
#
$kVersion = "v1.2";
$kFileSpecifier = ".php";
$kBalanceFileName = "c:\\Web\\ns\\Balance.txt";
$kTempFileName = "UpdateManual.temp";

local $blankLine = "....................................................................";

sub Main;
Main();

# Subroutine that checks file out of Perforce, does a search and replace on all values, then reverts the file if unchanged
sub ProcessFileName
{	
	# Read subroutine args
	my $theFileName = shift(@_);

	use POSIX qw(strftime);

	# Read in whole file
	my $printString = sprintf("READING  \"$theFileName\"");
	if(!open(FILE2, $theFileName))
	{
		printf("%s%s[FAILURE]\n",$printString,substr($blankLine,length($printString)));
		return 0;
	}

	printf("%s%s[SUCCESS]\n",$printString,substr($blankLine,length($printString)));

	# Open tempoarary file for writing
	if(!open(FILE3, '>', $kTempFileName))
	{
		$printString = sprintf("WRITING  \"$theFileName\"");
		printf("%s%s[FAILURE]\n",$printString,substr($blankLine,length($printString)));
		return 0;
	}

	# Check for a match
	my $theNumReplacements = 0;

	while(<FILE2>)
	{
		local $theCurrentLine = $_;

		if($theCurrentLine =~ s/(<ns:value name=\"([^\"]+)\">)[^<]*(<\/ns:value>)/$1 . $BalanceVars{$2} . $3/ge)
		{ $theNumReplacements++; }

		# This can't be in an else clause any more because both types may appear on the same line
		if($theCurrentLine =~ s/(<ns:percent name=\"([^\"]+)\">)[^<]*(<\/ns:percent>)/$1 . sprintf("%.0f%%",$BalanceVars{$2}*100) . $3/ge)
		{ $theNumReplacements++; }

		if($theCurrentLine =~ s/(<ns:inverse name=\"([^\"]+)\">)[^<]*(<\/ns:inverse>)/$1 . sprintf("%.2f",1\/$BalanceVars{$2}) . $3/ge)
		{ $theNumReplacements++; }

		if($theCurrentLine =~ s/(<ns:date format=\"([^\"]+)\">)[^<]*(<\/ns:date>)/$1 . strftime($2,@BalanceUpdateTime) . $3/ge)
		{ $theNumReplacements++; }

		print FILE3 $theCurrentLine;
	}

	close FILE2;
	close FILE3;

	if($theNumReplacements > 0) # Update if a match was present
	{
		$printString = sprintf("UPDATING \"$theFileName\"");
		
		if(unlink($theFileName) && rename($kTempFileName,$theFileName))
		{ 
			printf("%s%s[SUCCESS]\n",$printString,substr($blankLine,length($printString)));
			return 1;
		}
		else
		{ 
			printf("%s%s[FAILURE]\n",$printString,substr($blankLine,length($printString)));
			return 0;
		}
	}
	else
	{
		unlink($kTempFileName);
		return 1;
	}
}

sub ReadBalanceVariables
{
	# Read Balance file
	my $printString = sprintf("PARSING  \"$kBalanceFileName\"");
	open(FILE, $kBalanceFileName);

	@BalanceUpdateTime = localtime((stat(FILE))[9]);
	my $theSuccess = 0;

	# Print contents of Balance.txt
	while(<FILE>)
	{
		my $theCurrentBalanceLine = $_;
		if($theCurrentBalanceLine =~ /#define/)
		{
			my @theCurrentTokens;
			@theCurrentTokens = split();

			if($#theCurrentTokens == 2)
			{
				my $tokenOne = $theCurrentTokens[1];
				my $tokenTwo = $theCurrentTokens[2];

				$BalanceVars{$tokenOne} = $tokenTwo;

				$theSuccess = 1;
			}
		}
	}

	close(FILE); 

	printf("%s%s[%s]\n",$printString,substr($blankLine,length($printString)),$theSuccess ? "SUCCESS" : "FAILURE");
	return $theSuccess;
}

sub Main
{
	print "\nRunning Natural Selection Manual Updater $kVersion\n\n";

	if($#ARGV >= 0) # First arg is the Balance document name
	{
		$kBalanceFileName = $ARGV[0];
		shift(@ARGV); # Drop first argument
	}

	# Read in the contents of Balance.txt to use for replacement
	if(ReadBalanceVariables() == 0)	
	{ return; } # Return on failure

	if($#ARGV >= 0) # Remaining args are files to parse
	{ @theHTMLFiles = @ARGV }
	else
	{
		# Search through all the files in the working directory
		opendir(DIR, ".");
		local @AllFiles = readdir(DIR);
		@theHTMLFiles = grep(/$kFileSpecifier/i,@AllFiles);
		closedir(DIR);
	}

	# Process each file
	foreach (@theHTMLFiles)
	{ ProcessFileName($_); }
}