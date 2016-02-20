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

gcc -o vmerge main.c snplist.c extract.c merge.c awkvar.c pipeline.c