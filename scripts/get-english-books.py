#!/bin/python

# read the .csv file provided as command line argument
# print the first and 5th column

import csv
import sys

with open('./' + str(sys.argv[1]), 'r') as csvfile:
    reader = csv.reader(csvfile)
    for row in reader:
        print (row[0], row[5])
