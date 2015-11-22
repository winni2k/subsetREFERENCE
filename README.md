# subsetREFERENCE

This is a simple tool for subsetting sites from a
[WTCCC style haplotypes file](https://mathgen.stats.ox.ac.uk/genetics_software/shapeit/shapeit.html#hapsample).

## Authors

This code was originally written by Olivier Delaneau.  All changes
from the initial commit were made by
[Warren Kretzschmar](www.warrenwk.com).

For bugs please contact Warren Kretzschmar at `wkretzsch@gmail.com`,
or open an issue on github.

## Synopsis

./bin/subsetREFERENCE input.map input.hap.gz output.hap.gz

## Input

### .map file

The .map file is space separated and consists of the first five
columns of a VCF: Chromosome identifier, position, variant ID, ref
allele, alt allele.  The .map file contains no header. This is a valid
.map file:
````
20 60309 20:60309_G_T G T
20 60479 20:60479_C_T C T
20 60571 20:60571_C_A C A
20 60828 20:60828_T_G T G
````

### .hap.gz file

This is a
[WTCCC style haplotypes file](https://mathgen.stats.ox.ac.uk/genetics_software/shapeit/shapeit.html#hapsample).
This is a valid .hap.gz file:
````
20 20:60309_G_T 60309 G T 0 0 1 0
20 20:60571_C_A 60571 C A 0 0 0 0
````

## Output

The output is a WTCCC style haplotypes file that only includes sites
found in the .map file. Matching is performed on chromosome, position,
ref allele, and alt allele.  The variant ID is ignored.

## Other arguments

After the first three arguments, the following arguments may be given:

### `complement`

Providing this argument will cause matching to be reversed. Only sites
that are not in the .map file are output to the output.hap.gz file.

### `noStrand`

If a site is not found in the .map file, also check to see if the site
matches a site in the .map file with ref and alt allele swapped.





