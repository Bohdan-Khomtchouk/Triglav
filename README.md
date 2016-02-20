Triglav performs the following operations:

1) Generates a SNP list for every input file in your genomic sample pool
2) Extracts variants based on each of these SNP lists
3) Merges all extracted variants into one final file

Triglav outperforms the standard `vcf-merge` utility found in `vcftools`, which typically gives rise to problems when merging indels with normal vcf files.  Also, vcftools’ `vcf-merge` utility only takes the first values from the list of merged vcf files, thereby affecting other post-analysis scripts such as `VQSR` for downstream filtering of the data.  