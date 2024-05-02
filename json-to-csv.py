import json
import csv
import os

directory_path = os.path.dirname(os.path.realpath(__file__))
data_files = [f for f in os.listdir(directory_path) if f.endswith('-data')]

for filename in data_files:
    json_path = os.path.join(directory_path, filename)
    output_csv_path = os.path.join(directory_path, filename + '.csv')

    with open(json_path, 'r') as file:
        data = json.load(file)

    all_fields = set()
    for entry in data['benchmarks']:
        all_fields.update(entry.keys())
    fields_list = list(all_fields)

    with open(output_csv_path, mode='w', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=fields_list)
        writer.writeheader()
        for entry in data['benchmarks']:
            writer.writerow(entry)

    print(f"Data from {json_path} written to {output_csv_path}")
