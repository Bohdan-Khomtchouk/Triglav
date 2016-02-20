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

int merge_vcfs(char *config[])
{

	DIR *dir;
	struct dirent *ent;
	char *subit;
	char *subit2;
	FILE *logp;
	char description[100];
	char sample[50];
	char read1[1000];
	char read2[1000];
	char dir_trunk[] = "/smoke/hiseq_hip/analysis_runs/running/";
	char current_command[1000];
	char current_directory[1000];
	char rmdup_job_file[1000];
	char sys_command[1000];
	char run_dir[1000];
	int i;

	i = 0;
	/* create directory before starting */
	sprintf(current_directory,"%s%s_%s/run/rmdup/",dir_trunk,config[2],config[3]);
	printf("Current directory is \t%s\n",current_directory);
	dir = opendir (current_directory);
	if (dir != NULL) {

	  /* within the directory create the .job files to be put in the logs directory, submit them then sleep for 2 seconds*/
		while ((ent = readdir (dir)) != NULL) {
			/* check for the "chunk part of the file if it is there then proceed */
			subit = strstr(ent->d_name,"_final_");
			if (subit != NULL) {
				
				subit2 = strstr(ent->d_name,"bai");
				if (subit2 == NULL) {
				sprintf(rmdup_job_file,"%s%s_%s/logs/rmdup/rmdupJob%d.job",dir_trunk,config[2],config[3],i);
				logp = fopen(rmdup_job_file,"w");
				if (logp == NULL) {
					printf("could not open log file for writing\n");
				}
				else {
					printf("hey we wrote a log file\n");
					/* now we build the job */
					fprintf(logp, "#!/bin/bash\n");
					fprintf(logp, "#BSUB -J rmdupJob%d\n", i);
					fprintf(logp, "#BSUB -o %s%s_%s/logs/rmdup/rmdupJob%d.out\n", dir_trunk,config[2],config[3],i);
					fprintf(logp, "#BSUB -e %s%s_%s/logs/rmdup/rmdupJob%d.err\n", dir_trunk,config[2],config[3],i);
					fprintf(logp, "#BSUB -W 5:00\n");
					fprintf(logp, "#BSUB -n 1\n");
					fprintf(logp, "#BSUB -q small\n");
					fprintf(logp, "#BSUB -u dvanbooven\n");
					fprintf(logp, "#BSUB -R \"rusage[mem=4000:swp=4000]\"\n");
					fprintf(logp, "#BSUB -g /dh_aln_grp1\n\n");
					fprintf(logp, "java -jar /mihg/analysis/src/picard/picard-tools-1.42/MarkDuplicates.jar REMOVE_DUPLICATES=true VALIDATION_STRINGENCY=SILENT AS=true I=%s%s_%s/run/rmdup/%s_final_%d.bam O=%s%s_%s/run/sampbam/%s_final_%d.bam M=%s%s_%s/stats/%s.chunk%d.rmdup.picard.metrics.txt\n",dir_trunk,config[2],config[3],config[3],i,dir_trunk,config[2],config[3],config[3],i,dir_trunk,config[2],config[3],config[3],i);
					fprintf(logp, "samtools index %s%s_%s/run/sampbam/%s_final_%d.bam\n",dir_trunk,config[2],config[3],config[3],i);
					
				}
				fclose(logp);
				sprintf(sys_command,"bsub < %s%s_%s/logs/rmdup/rmdupJob%d.job",dir_trunk,config[2],config[3],i);
				system(sys_command);
				sprintf(sys_command,"touch %s%s_%s/logs/rmdup/.started",dir_trunk,config[2],config[3]);
				system(sys_command);
				sleep(1);
				i++;
				}
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}




	return 0;

}