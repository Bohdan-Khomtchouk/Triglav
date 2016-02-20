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
#include <unistd.h>

/* The purpose of this program is to merge a set of given vcf files into a single main vcf merged file.

 PIPELINE SETUP:

SNPLIST GENERATION - EXTRACTION OF VARIANTS - MERGE THE EXTRACTED FILES


*/

int init_new(char *config[]);
int awkvar(char *config[]);
int snplist_generation(char *config[]);
int extract_variants(char *config[]);
int merge_vcfs(char *config[]);
int pipeline(char *config[]);

int main(int argc, char *argv[])
{

	FILE *logfp;
	FILE *outfp;
	FILE *pfp;
	char filestring[50000];
	char *filelist;
	char cwd[255];
	char username[200];
	char * pfilestring;
	char * pfilestring2;
	char * end_str;

	long i;
	int j;
	int k;
	int n;
	int return_status;
        char monitorfile[] = "/smoke/dvanbooven/running/vcf_merge/current";
	char sys_command[1000];

	char *stages[] = { "snplist", "extract", "merge", "complete", "awkvar", "pipeline", "awk_snp", "ext_mer" };

	j = -1;
	for (n = 0; n < 8; n++) {
		if (strcmp(argv[1],stages[n]) == 0) { j = n; }
	}
	if (j == -1) {
		printf ("Function could not be determined.  Check and retry.\n");
		return 2;
	}


	/* This is the switch to determine which function to call and start processing. */
	switch (j) {

		case 0:
			printf("you want to start a new snplist run\n");
			return_status = snplist_generation(&argv[2]);
			break;
		case 1:
			printf("you want to start extracting a given list of snps from files\n");
			return_status = extract_variants(&argv[1]);
			break;
		case 2:
			printf("you want to start merging from a given group of zipped vcfs\n");
			return_status = merge_vcfs(&argv[1]);
			break;
		case 3:
			printf("you want to start a complete pipeline run\n");
			return_status = snplist_generation(&argv[1]);
			break;
		case 4:
			printf("you want to start a new snplist run\n");
			return_status = awkvar(&argv[2]);
			break;
		case 5:
			printf("you want to start a new pipeline run\n");
	                filelist = strrchr(argv[2],'/');
	                if (filelist == NULL) {
				getcwd(cwd,255);
        	                sprintf(filestring,"%s/%s",cwd,argv[2]);
               		}
			else {
				strcpy(filestring,argv[2]);
			}

			/* grab username of individual running the pipeline and write it to running */
			sprintf(sys_command,"whoami");
			
			pfp = popen("whoami","r");
			fgets(username,50000,pfp);
			username[strlen(username) - 1] = '\0';
			printf("username is %s\n",username);
			logfp = fopen(monitorfile,"a");
			fprintf(logfp,"%s\t1\t0\t%s\t%s\n",username,filestring,argv[3]);
		        fclose(logfp);
			printf("now call the first stage of the pipeline which is awkvar\n");
			getcwd(cwd,255);
			printf("current working directory is %s\n",cwd);
			//sprintf(sys_command,"/smoke/dvanbooven/development/new_merge/vmerge awkvar %s %s",argv[2],argv[3]);
			//system(sys_command);
			break;
		case 6:
                        printf("you want to start a snplist generation run\n");
                        filelist = strrchr(argv[2],'/');
                        if (filelist == NULL) {
                                getcwd(cwd,255);
                                sprintf(filestring,"%s/%s",cwd,argv[2]);
                        }
                        else {
                                strcpy(filestring,argv[2]);
                        }

                        /* grab username of individual running the pipeline and write it to running */
                        sprintf(sys_command,"whoami");

                        pfp = popen("whoami","r");
                        fgets(username,50000,pfp);
                        username[strlen(username) - 1] = '\0';
                        printf("username is %s\n",username);
                        logfp = fopen(monitorfile,"a");
                        fprintf(logfp,"%s\t2\t0\t%s\t%s\n",username,filestring,argv[3]);
                        fclose(logfp);
                        printf("now call the first stage of the pipeline which is awkvar\n");
                        getcwd(cwd,255);
                        printf("current working directory is %s\n",cwd);
                        //sprintf(sys_command,"/smoke/dvanbooven/development/new_merge/vmerge awkvar %s %s",argv[2],argv[3]);
                        //system(sys_command);
                        break;
		case 7:
                        printf("you want to start a merge run\n");
                        filelist = strrchr(argv[2],'/');
                        if (filelist == NULL) {
                                getcwd(cwd,255);
                                sprintf(filestring,"%s/%s",cwd,argv[2]);
                        }
                        else {
                                strcpy(filestring,argv[2]);
                        }

                        /* grab username of individual running the pipeline and write it to running */
                        sprintf(sys_command,"whoami");

                        pfp = popen("whoami","r");
                        fgets(username,50000,pfp);
                        username[strlen(username) - 1] = '\0';
                        printf("username is %s\n",username);
                        logfp = fopen(monitorfile,"a");
                        fprintf(logfp,"%s\t3\t0\t%s\t%s\n",username,filestring,argv[3]);
                        fclose(logfp);
                        printf("now call the first stage of the pipeline which is awkvar\n");
                        getcwd(cwd,255);
                        printf("current working directory is %s\n",cwd);
                        //sprintf(sys_command,"/smoke/dvanbooven/development/new_merge/vmerge awkvar %s %s",argv[2],argv[3]);
                        //system(sys_command);
                        break;




	}

	printf("Return status from working function:\t%d\n",return_status);


	return 0;

}