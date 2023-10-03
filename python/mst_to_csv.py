import xml.etree.ElementTree as et
import re
import argparse


def main():
    parser = argparse.ArgumentParser(description='Extract and write numbers from MST file to CSV file')
    parser.add_argument('input_file', help='Path to the input MST file')
    parser.add_argument('output_file', help='Path to the output CSV file')
    args = parser.parse_args()

    tree = et.parse(args.input_file)
    root = tree.getroot()

    csv_file = open(args.output_file, 'w', newline='', encoding='utf-8')

    number_pattern = re.compile(r'\d+')

    for variable in root.findall('.//variable'):
        name = variable.get('name')
        if name.startswith('w'):
            continue
        numbers = ','.join(number_pattern.findall(name))
        row_data = [numbers]
        csv_file.write(','.join(row_data) + '\n')

    csv_file.close()

if __name__ == "__main__":
    main()