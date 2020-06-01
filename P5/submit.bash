#!/bin/bash
#SBATCH -J MatrixMult 
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o p5.out
#SBATCH -e p5.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=yuanqi@oregonstate.edu
./config.sh
