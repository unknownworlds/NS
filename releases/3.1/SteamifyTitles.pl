#
# Updates titles.txt to Steam format.  Written by Charlie Cleveland.
#
local ($kVersion, $inputFilename, $modName, $languageName, $numTranslationsFound, $outputFilename, %translationData);

#
# Constants
#
$kVersion = "v1.0";
$inputFilename = "";
$modName = "";
$languageName = "";
$numTranslationsFound = 0;
$outputFilename = "";

sub Main;
Main();

sub Main
{
	print "\nRunning SteamifyTitles $kVersion by Charlie Cleveland (flayra\@overmind.org)\n\n";

	# Read mod name, input filename and language name
	$numArgs = $#ARGV + 1;

	if($numArgs < 3)
	{
		print "Usage: <input titles.txt> <mod name> <language name> <optional \"y\" if you want to convert input titles.txt>";
		print "Run from within your mod directory.\n"
	}
	else
	{
		# Parse arguments
		$inputFilename = $ARGV[0];
		$modName = $ARGV[1];
		$languageName = $ARGV[2];
		$convertTitles = 0;
		
		if($numArgs > 3)
		{
			if($ARGV[3] == "y")
			{
				$convertTitles = 1;
			}
		}

		#print "\nReading $inputFilename, for mod $modName, in language $languageName...\n";

		# Read in file name specified on command line
		print "Reading old titles from $inputFilename.\n";
		open(INFILE, $inputFilename) or die "Couldn't open $inputFilename for reading.\n";

		# Read all data into one string
		local $/;
		my $theData = <INFILE>;
		#print $theData;

		#while( $theData =~ m/ (\w+) \s* \{(.*?)\} /sgx) 
		while( $theData =~ m/ (\w+) \s* \{(.*?)\} /sgx) 
		{
			my $theKey = $1;
			my $theUntrimmedValue = $2;
			if($theUntrimmedValue =~ s/(.*?)\n+/ /g)
			{
				my $theTrimmedData = $1;

				# Skip comments?
				#if(index($theKey, "//") != -1)
				#{
				#	print "Skipping line: \"$theKey\" = \"$theTrimmedData\"\n";
				#}
				#else
				#{
					#print "Found pair: \"$theKey\" = \"$theTrimmedData\"\n";
							
					$translationData{$theKey} = $theTrimmedData;
					$numTranslationsFound++;
				#}
			}
		}

		close(INFILE);

		print "Found $numTranslationsFound translations.\n";

		if($numTranslationsFound > 0)
		{
			# Open file for writing as resource/modname_language.txt
			$outputFilename = lc("resource/$modName") . "_" . lc($languageName) . ".txt";
			print "Writing $outputFilename.\n";

			open(OUTFILE, ">$outputFilename") or die "Couldn't open file $outputFilename for writing.\n";
			
			my $kLineDelimiter = "\n";
			my $kLineSpaceDelimiter = " \n";
			my $kPairSeparator = "\t\t\t";

			# Write "lang" tag
			print OUTFILE "\"lang\"$kLineSpaceDelimiter";

			my $caseProperLanguage = lc($languageName);
			$caseProperLanguage = ucfirst($caseProperLanguage);

			# Write "Language" tag
			print OUTFILE "{$kLineSpaceDelimiter";
			print OUTFILE "\"Language\"$kPairSeparator\"$caseProperLanguage\"$kLineSpaceDelimiter";

			# Write "Tokens" tag
			print OUTFILE "\"Tokens\"$kLineSpaceDelimiter\{$kLineSpaceDelimiter";

			# For each translation pair
			my $key;
			my $value;

			# sort( keys( %hash ))
			#while(($key, $value) = each %translationData)
			foreach my $key ( sort(keys(%translationData)))
			{
				my $value = $translationData{$key};
				
				# Substitute any " for ', or HL stops reading the translation file
				$value =~ s/\"/\'/g;

				if(length($value) == 0)
				{
					print (" Skipping empty entry for \"$key\"\n");
				}
				else
				{
					my $theOutputString = "\"$key\"$kPairSeparator\"$value\"$kLineDelimiter";
					#print $theOutputString;
					print OUTFILE $theOutputString;
				}
			}

			# Write ending braces
			print OUTFILE "}$kLineSpaceDelimiter";
			print OUTFILE "}$kLineSpaceDelimiter";
			print OUTFILE "$kLineDelimiter$kLineDelimiter$kLineDelimiter";

			# close output file
			#print("Closing $outputFilename...");

			close(OUTFILE);

			# if we're writing new titles.txt
			if($convertTitles == 1)
			{
				my $newTitlesName = "titles.txt";
				print("Writing $newTitlesName in VGUI2 format...");
				open(TITLESFILE, ">$newTitlesName") or die "Couldn't open $newTitlesName for writing.\n";

				foreach my $key ( sort(keys(%translationData)))
				{
					print TITLESFILE "$key\n";
					print TITLESFILE "\{\n\#$key\n\}\n\n";
				}
				print "done.\n";

				close(TITLESFILE);
			}

			print "Done.  Don't forget to save \"$outputFilename\" as Unicode using Notepad before using.\n";
		}
	}
}

