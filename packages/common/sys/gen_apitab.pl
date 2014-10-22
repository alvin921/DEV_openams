#!/usr/local/bin/perl -w

use strict;
use Cwd;

require "$ENV{PJTOOLS}/prod.pm";

#--------------------------
#---- Global variables ----
#--------------------------

#-------------------------------------------------------

#====================================================================
#=== MAIN ====
#====================================================================

{
#  close STDOUT;
#  open STDOUT,">/dev/stderr";

	my @AllModules;# = ("button", "editor", "listctrl", "menu", "softkey", "titlebar", "label", "nesctrl");
	my @tmp;
	my ($m, $li, $def, $path);
	my (%modpath);

	#ParseFMC("$ENV{PJTOOLS}\\findmodules.cache", \@tmp, \@AllModules);
	@AllModules = GetModuleList("Makefile");
	%modpath = LoadConfig("$ENV{PJTOOLS}\\findmodules.cache");
	
	print "%%\n%% Generating api table...\n";

	open(CFILE, ">pack_api_table.c") || die "error to create api_table.c\n";

	print CFILE "#include \"ams.h\"\n";
	foreach $m (@AllModules) {
		next if($m =~ /^sys$/);
		print CFILE "#include \"wgt_$m.h\"\n";
	}
	print CFILE "#include \"pack_sys.h\"\n";
	print CFILE "const gu8 * const PackSysApiTable[]={\n";

	push @AllModules, "sys";
	foreach $m (@AllModules) {
		$path = $modpath{$m};
		$def = "$path\\vcproj\\dll.def";
		open(DEF, "$def") || print("failed open $def\n") && next;
		foreach $li (<DEF>){
			$li =~ s/[\012\015\t\s]//g;
			next if($li =~ /EXPORTS/);
			
			next if($li =~ /DATA/);

			if($li =~ /^(\w+)$/){
				print CFILE "\t(const gu8*)$1,\n";
			}
		}
		close(DEF);
	}

	print CFILE "};\n";
	print CFILE "const gu32 PackSysApiNum = ARRAY_SIZE(PackSysApiTable);\n";
	close(CFILE);

	mySyst("move /Y pack_api_table.c srce\\");
	

}

