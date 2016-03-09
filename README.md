# Triglav

## About

Triglav is a bioinformatics software program written in the C programming language that performs the following operations:

* Generates a single nucleotide polymorphism (SNP) list for every input file in your genomic sample pool
* Extracts variants based on each of these SNP lists
* Merges all extracted variants into one final file

Triglav outperforms the standard `vcf-merge` utility found in `vcftools`, which typically gives rise to problems when merging indels with normal variant calling format (vcf) files.  Also, the `vcf-merge` utility only takes the first values (in the INFO field) from the list of merged vcf files, thereby affecting other post-analysis scripts such as `VQSR` for downstream filtering of the data. 

Current work is underway to transition Triglav from an HPC environment to a cloud infrastructure.  Also, an indel-merge feature is currently under development and testing.  

## Funding

Triglav is an ongoing bioinformatics software project financially supported by the
United States Department of Defense (DoD) through the National Defense Science and Engineering
Graduate Fellowship (NDSEG) Program. This research was conducted with Government support under
and awarded by DoD, Army Research Office (ARO), National Defense Science and Engineering
Graduate (NDSEG) Fellowship, 32 CFR 168a.
