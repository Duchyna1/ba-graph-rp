#!/bin/bash

# Splits a file into the given number of parts and executes a command for each part in a separate directory.
# Useful for dealing with large graph6 files when you have several cores available.

# Ensure the script exits on any errors
set -e

# Check if user provided necessary arguments
if [ $# -ne 2 ]; then
  echo "Usage: $0 <datafile> <N>"
  exit 1
fi

# Assign variables
datafile=$1      # Input data file (e.g., datafile.txt)
N=$2             # Number of parts to split the datafile into
base_dir=$(pwd)  # Current working directory
common_files_dir="$base_dir/common_files" # Directory containing common files

# Split the datafile into N parts (N files)
# -d: use numeric suffixes, e.g., part_00, part_01
# --additional-suffix=.part: add .part extension to the files
# -n l/N: split file into N parts without breaking lines
split -n l/$N --numeric-suffixes=1 --suffix-length 4 --additional-suffix=.part "$datafile" part_

# Iterate over each split part and create necessary directories
i=1
for part in part_*.part; do
  # Create a directory for the part
  part_dir=$(printf "part_%04d" $i)
  mkdir -p "$part_dir"

  # Move the split file into its respective directory
  mv "$part" "$part_dir/"

  # Copy common files into the directory
  cp -r "$common_files_dir"/* "$part_dir/"

  # Customize and run the command with nohup
  # Example command: "your_command --input=part_$i"
  command="./hcolouring 2C3 $part"

  # Run the command in the background with nohup
  (cd "$part_dir" && nohup $command > nohup_output.txt 2> nohup_error.txt &)

  # Increment part counter
  ((i++))
done

echo "Splitting and copying completed. Command execution started."
