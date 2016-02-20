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
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char *argv[])
{

	DIR *dir;
	struct dirent *ent;
	char *filename;
	char *filelist;
	char *status;
	char *working_dir;
	char *username;
	char *ptype;
	FILE *logp;
	FILE *filelistfp;
	FILE *jobptr;
	FILE *outptr;
	char *end_str;
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
	int awkvar_started;
	int awkvar_finished;
	int snplist_started;
	int snplist_finished;
	int extract_started;
	int extract_finished;
	int merge_started;
	int merge_finished;
	int check_status;

	i = 0;
	
	/* Go through each of the lines in the filelist.txt file and awk out the variants into the Variants Directory */
	filelistfp = fopen("/hihg/smoke/pipelines/vcf","r");
	if (filelistfp == NULL) {
		printf ("The current pipeline runs list could not be opened.  Exiting \n");
		return 2;
	}
	fgets(filestring,50000,filelistfp);
	while (fgets(filestring,50000,filelistfp) != NULL) {
		/* for each line in the pipeline run list go through and see what is not running */
		/* convention for the currnet file is this: USERNAME - TYPE - STATUS - FILELIST - WORKING_DIR */
		
		/* chop up the line and separate the lists */
                username = strtok_r(filestring,"\t\n",&end_str);
                ptype = strtok_r(NULL,"\t\n",&end_str);
                status = strtok_r(NULL,"\t\n",&end_str);
		filelist = strtok_r(NULL,"\t\n",&end_str);
		working_dir = strtok_r(NULL,"\t\n",&end_str);

		printf("status is %s\n",status);

		/* if status is 0 then it is assumed to be running at some stage so figure out the stage*/

		if (strcmp(status,"0") == 0) {
			printf("we have a 0 status running pipeline job\n");
			/* now get the type of pipeline job this is */
			/* check started and finished flags of each stage */
			
			sprintf(current_directory,"%s/Variants/logs/.started",working_dir);
			awkvar_started = check_file(current_directory);
			sprintf(current_directory,"%s/Variants/logs/.finished",working_dir);
			awkvar_finished = check_file(current_directory);
			sprintf(current_directory,"%s/Positions/logs/.started",working_dir);
			snplist_started= check_file(current_directory);
			sprintf(current_directory,"%s/Positions/logs/.finished",working_dir);
			snplist_finished = check_file(current_directory);
			sprintf(current_directory,"%s/VCF_files/logs/.started",working_dir);
			extract_started = check_file(current_directory);
			sprintf(current_directory,"%s/VCF_files/logs/.finished",working_dir);
			extract_finished = check_file(current_directory);
			sprintf(current_directory,"%s/final/logs/.started",working_dir);
			merge_started = check_file(current_directory);
			sprintf(current_directory,"%s/final/logs/.finished",working_dir);
			merge_finished = check_file(current_directory);
			
			if (awkvar_started == 1 && awkvar_finished == 0) {
				check_status = check_logs(working_dir);
				/* check for Exited tag in any of the jobs */
			}
		}
		else {
			printf("we have a finished pipeline run\n");
		}
		/* now tha tyo uhave the stage */

		
		
	}
	
	return 0;

}


int check_file(char *working_dir)
{

	char current_directory[1000];
	FILE *logp;

	//sprintf(current_directory,"%s/Positions/.started",working_dir);
	logp = fopen(working_dir,"r");
	if (logp == NULL) { return 0; }
	else { fclose(logp); return 1; }

}


int check_logs(char *working_dir)
{

	FILE *jobptr;
	FILE *logp;
	char sys_command[1000];
	char current_directory[2000];
	char filestring[5000];
	char filestring2[5000];
	char filestring3[5000];
	struct stat st;

	/* This function checks the job files and the out files and makes sure they are equal.
	If they are equal then a grep will be performed to check to see if anything exited
	If something exited then it will be resubmitted 1 time.
	The original job file will be renamed with the extension of originalfilename.job.exited */

	/* if the second job exits then the status will be changed in current to reflect the status = 1 */


	sprintf(sys_command,"ls %s/Variants/logs/*job | wc -l",working_dir);
	printf("syscommand is %s\n",sys_command);
	jobptr = popen( sys_command, "r");
	if (fgets(filestring,5000,jobptr) == NULL) {
		printf("nothing returned\n");
	}
	else {
		printf("list count is %s\n",filestring);
	}
	fclose(jobptr);

	sprintf(sys_command,"ls %s/Variants/logs/*out | wc -l",working_dir);
        printf("syscommand is %s\n",sys_command);
        jobptr = popen( sys_command, "r");
        if (fgets(filestring2,5000,jobptr) == NULL) {
                printf("nothing returned\n");
        }
        else {
                printf("list count is %s\n",filestring2);
        }

	if (strcmp(filestring,filestring2) == 0) {
		/* There is the same number so do a grep and see if anything exited */
		sprintf(sys_command,"grep Exited %s/Variants/logs/*",working_dir);
		printf("sys command is %s\n",sys_command);
		logp = popen(sys_command, "r");
		
		if (fgets(filestring3,5000,logp) == NULL) {
			/* Everything appears to have successfully completed so return that value */
			printf("everything worked\n");
			fclose(jobptr);
			return 1;
		}
		else {

			/* Check to see if a failed directory exists.  If it does not then we are on the first round.
			Create the directory.  Move the failed jobs into that directory.  Copy the .jobs from that directory
			into the directory one level up.  Submit those jobs again. */

			printf("the logp is not null\n");	
			sprintf(current_directory,"%s/Variants/logs/failed/",working_dir);
			
			if(stat(current_directory,&st) == 0) {
				/* Directory is present so exit this with return message to stop the pipeline since we have 2 failed jobs. */
				printf("failed job and failed directory so exit out \n");
				return 5;
			}
			else {
				printf("current directory is %s\n",current_directory);
				sprintf(sys_command,"mkdir %s/Variants/logs/failed",working_dir);
				system(sys_command);	
			}
		}
			
	}
	fclose(jobptr);

}