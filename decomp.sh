#!/bin/bash

for FILE in RunSupratikBenchmarks/*.qdimacs
do
	export FILENAME=$(basename ${FILE//.qdimacs/})
	sbatch decomp.slurm
done
