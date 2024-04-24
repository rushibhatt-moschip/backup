import csv
import subprocess

def read_testcase_from_csv(filename, n):
    with open(filename, 'r') as file:
        reader = csv.reader(file)
        testcases = list(reader)
        if n < 0 or n > len(testcases):
            print("Invalid testcase number.")
            return None
        print(f"Property names: {testcases[0]}")
        return testcases[n]

def main():
    csv_filename = 'test-cases.csv'							#testcases.csv ??

    n = int(input("Enter the nth testcase to run: "))
    testcase = read_testcase_from_csv(csv_filename, n)
    if testcase is not None:
        input_values = testcase  # Assuming each value in the CSV is an input value
        print(f"Input values: {input_values}")						#f ??
        subprocess.run(["./testapp", *input_values])

if __name__ == "__main__":
    main()

