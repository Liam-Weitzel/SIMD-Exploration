import pandas as pd
import glob
import os
import json
import logging

# Set up basic configuration for logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# Path to the directory containing JSON files
directory_path = '.'

# Pattern to match JSON files ending with '-data'
file_pattern = os.path.join(directory_path, '*-data')

# DataFrame to store all data
all_data = pd.DataFrame()

# Process each file in the directory that matches the pattern
for file_path in glob.glob(file_pattern):
    logging.info(f'Starting to process file: {file_path}')

    # Extract the library name from the file path
    library_name = os.path.basename(file_path).replace('-data', '')

    # Load the JSON file
    with open(file_path, 'r') as file:
        data = json.load(file)
        logging.info(f'Loaded data from {file_path}')

    # Extract benchmark data from the top-level 'benchmarks'
    benchmarks = data['benchmarks']
    for benchmark in benchmarks:
        # Extract execution time (cpu_time) and benchmark name
        execution_time = benchmark.get('cpu_time', None)
        benchmark_name = benchmark.get('name', None)

        # Append to the DataFrame
        all_data = pd.concat([all_data, pd.DataFrame({
            'library': [library_name],
            'execution time': [execution_time],
            'benchmark': [benchmark_name]
        })], ignore_index=True)

    logging.info(f'Finished processing file: {file_path}')

# Define the output file name for the consolidated data
output_file_name = 'consolidated_data_2.csv'

# Save the DataFrame to a CSV file with index
all_data.to_csv(output_file_name, index=True, index_label='Index')
logging.info(f'All data has been processed and consolidated into {output_file_name}')
