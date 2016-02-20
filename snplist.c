/* Copyright (C) 2016 Bohdan Khomtchouk and Derek Van Booven */

/* This file is part of Triglav. */

/* Triglav performs the following operations: */

/* 1) Generates a SNP list for every input file in your genomic sample pool */
/* 2) Extracts variants based on each of these SNP lists */
/* 3) Merges all extracted variants into one final file */

/* Triglav outperforms the standard vcf-merge utility found in vcftools, */ 
/* which typically gives rise to problems when merging indels with normal */
/* vcf files.  Also, vcftools’ vcf-merge utility only takes the first values */
/* from the list of merged vcf files, thereby affecting other post-analysis */ 
/* scripts such as `VQSR` for downstream filtering of the data. */ 

/* MicroScope is an ongoing bioinformatics software project financially supported by the United States Department of Defense (DoD) */ 
/* through the National Defense Science and Engineering Graduate Fellowship (NDSEG) Program. This research was conducted with Government support */ 
/* under and awarded by DoD, Army Research Office (ARO), National Defense Science and Engineering Graduate (NDSEG) Fellowship, 32 CFR 168a. */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU General Public License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* ------------------------------------------------------------------------------------ */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int snplist_generation(char *config[])
{

	DIR *dir;
	struct dirent *ent;
	char *subit;
	char *subit2;
	char *filename;
	FILE *logp;
	FILE *filelistfp;
	char description[100];
	char sample[50];
	char read1[1000];
	char read2[1000];
	char current_command[1000];
	char dir_trunk[1000];
	char filestring[50000];
	char current_directory[1000];
	char rmdup_job_file[1000];
	char snplist_job_file[1000];
	char sys_command[1000];
	char run_dir[1000];
	int i;

	i = 0;
	printf("within snplist_generation\n");
	/* create directory before starting */
	sprintf(current_directory,"%s",config[1]);
	printf("Current directory is \t%s\n",current_directory);
	/* First thing to do is to create a Positions directory for the outputted awked files */
	sprintf(sys_command,"mkdir %s/Positions",current_directory);
	system(sys_command);
	sprintf(sys_command,"mkdir %s/Positions/logs",current_directory);
	system(sys_command);
	
	/* Go through each of the lines in the filelist.txt file and awk out the variants into the Positions Directory */
	filelistfp = fopen(config[3],"r");
	printf("filelist is %s\n",config[3]);
	if (filelistfp == NULL) {
		printf ("The filelist could not be opened.  Exiting \n");
		return 2;
	}
	printf("whoa\n");
	while (fgets(filestring,50000,filelistfp) != NULL) {
		/* for each file in the Variants directory now extract the positions, cat all together and uniq to get snplist */
		printf ("within filelist\n");

		/* First thing to do is to do an effective chomp and get rid of newline character */
		filestring[strlen(filestring) - 1] = '\0';

                /* Now get the proper filename from the list.  Remember it could be either just a relative file or absolute path
                If absolute path then we need to parse out.  Search for the last instance of the '/' character and grab the rest */
                filename = strrchr(filestring,'/');
                if (filename == NULL) {
                        filename = filestring;
                }
                else {
                        filename++;
                }

		/* There is no LSF interaction for this at all.  This is within a main single job submitted to the cluster*/
		/* Now build the job files and submit to LSF*/

		sprintf(sys_command,"awk '!/#/ { print $1 \"\\t\" $2 } ' %s/Variants/%s > %s/Positions/%s",config[1],filename,config[1],filename);
		system(sys_command);
		printf("sys command is %s\n",sys_command);

		sleep(1);
		i++;
		
	}

	/* Now all the positions have been extracted it's time to concatenate all the vcfs */
	sprintf(sys_command,"cat %s/Positions/*vcf > %s/Positions/all.vcf.positions",config[1],config[1]);
	system(sys_command);

	/* Change X's to 23's to form corrected sorted file */
	sprintf(sys_command,"sed -i 's/X/23/g' %s/Positions/all.vcf.positions",config[1],config[1]);
	system(sys_command);

	/* Change Y's to 24's to form corrected sorted file */
	sprintf(sys_command,"sed -i 's/Y/24/g' %s/Positions/all.vcf.positions",config[1],config[1]);
	system(sys_command);

	/* Now the concatenation has happened do the sort */
	sprintf(sys_command,"sort -k1,1n -k2,2n %s/Positions/all.vcf.positions > %s/Positions/all.vcf.positions.sorted",config[1],config[1]);
	system(sys_command);

	/* Change 23's back to X's */
	sprintf(sys_command,"sed -i 's/^23/X/g' %s/Positions/all.vcf.positions.sorted",config[1],config[1]);
	system(sys_command);

	/* Change 24's back to Y's */
	sprintf(sys_command,"sed -i 's/^24/Y/g' %s/Positions/all.vcf.positions.sorted",config[1],config[1]);
	system(sys_command);


	/* Now we have the sorted file do a uniq to get unique positions for merging */
	sprintf(sys_command,"uniq %s/Positions/all.vcf.positions.sorted > snplist.txt",config[1]);
	system(sys_command);
	
	return 0;

}