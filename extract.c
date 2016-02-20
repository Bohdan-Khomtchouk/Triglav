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

int extract_variants(char *config[])
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
	sprintf(current_directory,"%s",config[2]);
	printf("Current directory is \t%s\n",current_directory);
	/* First thing to do is to create a Variants directory for the outputted awked files */
	sprintf(sys_command,"mkdir %s/VCF_files",current_directory);
	system(sys_command);
	sprintf(sys_command,"mkdir %s/VCF_files/logs",current_directory);
	system(sys_command);
	
	/* Go through each of the lines in the filelist.txt file and awk out the variants into the Variants Directory */
	filelistfp = fopen(config[1],"r");
	if (filelistfp == NULL) {
		printf ("The filelist could not be opened.  Exiting \n");
		return 2;
	}
	while (fgets(filestring,50000,filelistfp) != NULL) {
		/* for each file create a job file containing the awk command that we will use and submit them all to LSF */
		/* NOTE:  This will be placed within the ex_var1 job group which has a max of 50 jobs to be running at once. */

		/* First thing to do is to do an effective chomp and get rid of newline character */
		filestring[strlen(filestring) - 1] = '\0';
		/* Now get the proper filename from the list.  Remember it could be either just a relative file or absolute path
		If absoulte path then we need to parse out.  Search for the last instance of the '/' character and grab the rest */
		filename = strrchr(filestring,'/');
		if (filename == NULL) {
			filename = filestring;
		}
		else {
			filename++;
		}
		/* Now build the job files and submit to LSF*/
		printf("filename is %s\n",filename);
		sprintf(snplist_job_file,"%s/VCF_files/logs/extractJob%d.job",config[2],i);
		logp = fopen(snplist_job_file,"w");
		if (logp == NULL) {
			printf("could not open log file for writing\n");
		}
		else {
			printf("hey we wrote a log file\n");
			/* now we build the job */
			fprintf(logp, "#!/bin/bash\n");
			fprintf(logp, "#BSUB -J extractJob%d\n", i);
			fprintf(logp, "#BSUB -o %s/VCF_files/logs/extractJob%d.out\n",config[2],i);
			fprintf(logp, "#BSUB -e %s/VCF_files/logs/extractJob%d.err\n",config[2],i);
			fprintf(logp, "#BSUB -W 5:00\n");
			fprintf(logp, "#BSUB -n 1\n");
			fprintf(logp, "#BSUB -q small\n");
			fprintf(logp, "#BSUB -u dvanbooven\n");
			fprintf(logp, "#BSUB -g /ex_var1\n\n");
			fprintf(logp, "/smoke/dvanbooven/extraction/extract_variants %s/snplist.txt %s %s/VCF_files/%s\n\n",config[2],filestring,config[2],filename);

		}
		fclose(logp);
		sprintf(sys_command,"bsub < %s/VCF_files/logs/extractJob%d.job\n",config[2],i);
		system(sys_command);

		sleep(1);
		i++;
		
	}
	
	return 0;

}